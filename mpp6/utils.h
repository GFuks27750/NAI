#ifndef   DATASET_H_
#define   DATASET_H_

typedef struct {
  int *sizes;
  int *vals;
} dataset_t;

typedef struct {
  size_t     capacity;
  size_t     count;
  dataset_t *items;
} datasets_t;

typedef struct {
  int capacity;
  int length;
  datasets_t sets;
} datafile_t;

bool read_datasets_txt(const char *file_path, datafile_t *out_datafile) {
  FILE *file = fopen(file_path, "rb");
  if (!file) return false;

  datafile_t datafile = {0};
  datasets_t datasets = {0};
  dataset_t *cur_dataset = NULL;

  char line[256];
  int dataset_index = -1;
  int item_index = 0;

  bool startFound = false;

  while (fgets(line, sizeof(line), file)) {
    if (strstr(line, "length") == line) {
      if (startFound) return false;
      startFound = true;

      char *str = malloc(strlen(line));
      memcpy(str, line, strlen(line));
      str[strlen(line)-1] = '\0';
      char *ptr = strchr(str, ',');
      *ptr = '\0';
      ptr = strrchr(str, ' ');
      if (!ptr) ptr = strrchr(str, '-');
      ++ptr;
      datafile.length = atoi(ptr);
      ptr = str+strlen(str)+1;
      ptr = strrchr(ptr, ' ');
      datafile.capacity = atoi(ptr);
    } else if (strstr(line, "dataset") == line) {
      if (!startFound) return false;
      dataset_t set = {0};
      set.sizes = malloc(sizeof(int)*datafile.length);
      set.vals = malloc(sizeof(int)*datafile.length);
      nob_da_append(&datasets, set);
      cur_dataset = datasets.items + (datasets.count-1);
    } else if (strstr(line, "sizes") == line) {
      if (!cur_dataset) return false;
      char *sizes_str = strchr(line, '{') + 1;
      sizes_str[strlen(sizes_str) - 2] = '\0';
      char *token = strtok(sizes_str, ",");
      item_index = 0;
      while (token) {
        cur_dataset->sizes[item_index++] = atoi(token);
        token = strtok(NULL, ",");
      }
      if (item_index != datafile.length) return false;
    } else if (strstr(line, "vals") == line) {
      if (!cur_dataset) return false;
      char *vals_str = strchr(line, '{')+1;
      vals_str[strlen(vals_str)-2] = '\0';
      char *token = strtok(vals_str, ",");
      item_index = 0;
      while (token) {
        cur_dataset->vals[item_index++] = atoi(token);
        token = strtok(NULL, ",");
      }
      if (item_index != datafile.length) return false;
    }
  }

  datafile.sets = datasets;
  *out_datafile = datafile;

  fclose(file);
  return true;
}

bool read_datasets_bin(const char *file_path, datafile_t *out_datafile) {
  FILE *file = fopen(file_path, "rb");
  if (!file) {
    return false;
  }

  datafile_t datafile = {0};

  fread(&datafile.length, sizeof(datafile.length), 1, file);
  fread(&datafile.capacity, sizeof(datafile.capacity), 1, file);
  if (fgetc(file) != '\0') return false;
  
  while (true) { // Read datasets
    dataset_t set = {0};
    set.sizes = malloc(sizeof(*set.sizes) * datafile.length);
    set.vals  = malloc(sizeof(*set.vals) * datafile.length);

    {
      int *ptr = set.sizes;
      while (1) {
        fread(ptr++, sizeof(*ptr), 1, file);
        fseek(file, 0, SEEK_CUR);
        long save = ftell(file);
        if ('\0' == fgetc(file)) break; // Skip save position
        fseek(file, save, SEEK_SET);
      }
      if (ptr - set.sizes != datafile.length) return false;
    }

    {
      int *ptr = set.vals;
      while (1) {
        fread(ptr++, sizeof(*ptr), 1, file);
        fseek(file, 0, SEEK_CUR);
        long save = ftell(file);
        if ('\0' == fgetc(file)) break;
        fseek(file, save, SEEK_SET);
      }
      if (ptr - set.vals != datafile.length) return false;
    }

    nob_da_append(&datafile.sets, set);

    fseek(file, 0, SEEK_CUR);
    long save = ftell(file);
    if ('\0' == fgetc(file)) break;
    fseek(file, save, SEEK_SET);
  }

  *out_datafile = datafile;

  return true;
}

#endif // DATASET_H_