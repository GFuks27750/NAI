//
// converter.c - Program, whose sole purpose is to converting lame-ass txt files into blaming hot binary files 🥵.
//
// Usage: <program> <in.txt> <out.bin>
//

#define NOB_IMPLEMENTATION
#include "../include/nob.h"
#include "../include/utils.h"

#include "utils.h"

void usage(const char *const program) {
  nob_log(NOB_ERROR, "Usage: %s <in.txt> [data.bin]", program);
}

bool write_binary(FILE *fd, datafile_t datafile) {
  fwrite(&datafile.length, sizeof(datafile.length), 1, fd);
  fwrite(&datafile.capacity, sizeof(datafile.capacity), 1, fd);
  fputc('\0', fd);

  for (size_t i = 0; i < datafile.sets.count; ++i) {
    dataset_t set = datafile.sets.items[i];
    for (size_t j = 0; j < datafile.length; ++j) {
      fwrite(&set.sizes[j], sizeof(*set.sizes), 1, fd);
    }
    fputc('\0', fd);
    for (size_t j = 0; j < datafile.length; ++j) {
      fwrite(&set.vals[j], sizeof(*set.vals), 1, fd);
    }
    fputc('\0', fd);
  }
  fputc('\0', fd);

  return true;
}

int main(int argc, char **argv) {
  const char *const program = nob_shift_args(&argc, &argv);

  if (argc <= 0 || argc > 2) {
    usage(program);
    return 1;
  }

  const char *const in_file_name = nob_shift_args(&argc, &argv);
  char *out_file_name = NULL;
  if (argc > 0) {
    out_file_name = nob_shift_args(&argc, &argv);
  } else {
    Nob_String_Builder  out_sb = {0};
    nob_sb_append_cstr(&out_sb, get_dir_name(in_file_name));
    nob_sb_append_cstr(&out_sb, "data.bin");
    nob_sb_append_null(&out_sb);
    out_file_name = malloc(out_sb.count);
    memcpy(out_file_name, out_sb.items, out_sb.count);
    nob_sb_free(out_sb);
  }

  datafile_t datafile = {0};
  if (!read_datasets_txt(in_file_name, &datafile)) {
    nob_log(NOB_ERROR, "Failed to open/parse file `%s`!", in_file_name);
    return 1;
  }

  FILE *out_fd = fopen(out_file_name, "wb");
  if (!out_fd) {
    nob_log(NOB_ERROR, "Failed to open file `%s`!", out_file_name);
    return 1;
  }

  if (!write_binary(out_fd, datafile)) {
    nob_log(NOB_ERROR, "Failed to write to file `%s`!", out_file_name);
    return 1;
  }
  
  nob_log(NOB_INFO, "Successfully converted `%s` into `%s`!", in_file_name, out_file_name);

  fclose(out_fd);

  return 0;
}