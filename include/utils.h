//
// utils.h - Utilities
//

#ifndef   UTILS_H_
#define   UTILS_H_

char *get_dir_name(const char *const filename) {
  char* str = malloc(strlen(filename)+1);
  memcpy(str, filename, strlen(filename)+1);
  char* str_start = str;

  {
    char *save = str;
    str = strrchr(str, '/');
    if (!str) str = strrchr(save, '\\');
    if (str) str++;
    else str = (char*)filename;
  }

  *str = '\0';
  return str_start;
}

char *find_file(const char *filename, const char *dir) {
  Nob_File_Paths paths = {0};
  if (!nob_read_entire_dir(dir, &paths)) return NULL;

  for (size_t i = 0; i < paths.count; ++i) {
    const char *path = paths.items[i];
    if (strlen(path) < strlen(filename)) continue;

    char *file_name = malloc(strlen(path)+1);
    strcpy(file_name, path);
    file_name[strlen(path)] = '\0';
    char *ext = strrchr(file_name, '.');
    if (!ext) { free(file_name); continue; }
    *ext = '\0';

    if (strcmp(file_name, filename) == 0) {
      char *full_name = malloc(1 + strlen(dir) + strlen(path));
      strcpy(full_name, dir);
      strcpy(full_name+strlen(dir), path);
      full_name[strlen(dir)+strlen(path)] = '\0';
      free(file_name);
      return full_name;
    }

    free(file_name);
  }

  return NULL;
}

#endif // UTILS_H_