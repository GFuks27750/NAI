#define NOB_IMPLEMENTATION
#include "../include/nob.h"
#include <time.h>

#include "utils.h"

void knapsack_brute_force(int *sizes, int *vals, int n, int capacity, int *best_combination, int *max_value, int *count) {
  *max_value = 0;
  *count = 0;

  for (int r = 1; r < (1 << n); ++r) {
    int total_size = 0;
    int total_value = 0;
    for (int i = 0; i < n; ++i) {
      if (r & (1 << i)) {
        total_size += sizes[i];
        total_value += vals[i];
      }
    }
    ++(*count);
    if (total_size <= capacity && total_value > *max_value) {
      *max_value = total_value;
      memcpy(best_combination, &r, sizeof(int));
    }
  }
}

void knapsack_heuristic(int *sizes, int *vals, int n, int capacity, int *selected_items, int *total_value, int *selected_count) {
  float *ratios  = malloc(sizeof(*ratios)  * n);
  int   *indices = malloc(sizeof(*indices) * n);

  for (int i = 0; i < n; ++i) {
    ratios[i] = (float)vals[i] / sizes[i];
    indices[i] = i;
  }

  for (int i = 0; i < n-1; ++i) {
    for (int j = 0; j < n-i-1; ++j) {
      if (ratios[j] < ratios[j+1]) {
        float temp_ratio = ratios[j];
        ratios[j] = ratios[j+1];
        ratios[j+1] = temp_ratio;

        int temp_index = indices[j];
        indices[j] = indices[j+1];
        indices[j+1] = temp_index;
      }
    }
  }

  *total_value = 0;
  int total_size = 0;
  *selected_count = 0;
  for (int i = 0; i < n; ++i) {
    int index = indices[i];
    if (total_size + sizes[index] <= capacity) {
      selected_items[(*selected_count)++] = index;
      total_size += sizes[index];
     *total_value += vals[index];
    }
  }

  free(indices);
  free(ratios);
}

int main(int argc, char **argv) {
  srand(time(NULL));

  const char *const program = nob_shift_args(&argc, &argv);

  if (argc <= 0 || argc > 1) {
    nob_log(NOB_ERROR, "Usage: %s [data.bin]", program);
    return 1;
  }

  const char *file_name = NULL;
  if (argc == 1) {
    file_name = nob_shift_args(&argc, &argv);
  } else {

  }

  datafile_t datafile = {0};
  if (!read_datasets_bin(file_name, &datafile)) {
    nob_log(NOB_ERROR, "Failed to read file `%s`!", file_name);
    return 1;
  }

  int dataset_index = 8;
  dataset_t dataset = datafile.sets.items[dataset_index];
  int *sizes = dataset.sizes;
  int *vals = dataset.vals;

  { // Brute force solution
    int best_combination = 0;
    int max_value = 0;
    int count = 0;
    clock_t start_time = clock();
    knapsack_brute_force(sizes, vals, datafile.length, datafile.capacity, &best_combination, &max_value, &count);
    clock_t end_time = clock();
    double brute_force_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("Selected dataset: %d\n", dataset_index+1);
    printf("Brute Force Solution:\n");
    printf("Items:\n  ");
    int j = 0;
    for (int i = 0; i < datafile.length; ++i) {
      if (best_combination & (1 << i)) {
        if (j++ > 0) printf(", ");
        printf("(%d, %d, %d)", i+1, sizes[i], vals[i]);
      }
    }
    printf("\nTotal Value: %d\n", max_value);
    printf("Total Size: ");
    int total_size = 0;
    for (int i = 0; i < datafile.length; ++i) {
      if (best_combination & (1 << i)) {
        total_size += sizes[i];
      }
    }
    printf("%d\n", total_size);
    printf("Count of checked combinations: %d\n", count);
    printf("Execution Time: %f seconds\n", brute_force_time);
  }

  { // Heuristic solution
    int selected_items[datafile.length];
    int heuristic_value;
    int selected_count;
    clock_t start_time = clock();
    knapsack_heuristic(sizes, vals, datafile.length, datafile.capacity, selected_items, &heuristic_value, &selected_count);
    clock_t end_time = clock();
    double heuristic_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("\nHeuristic Solution:\n");
    printf("Items:\n  ");
    for (int i = 0; i < selected_count; ++i) {
      if (i > 0) printf(", "); printf("(%d, %d, %d)", selected_items[i] + 1, sizes[selected_items[i]], vals[selected_items[i]]);
    }
    printf("\nTotal Value: %d\n", heuristic_value);
    printf("Total Size: ");
    int total_size = 0;
    for (int i = 0; i < selected_count; ++i) {
      total_size += sizes[selected_items[i]];
    }
    printf("%d\n", total_size);
    printf("Execution Time: %f seconds\n", heuristic_time);
  }

  { // ~~I fleed from prison~~Free.
    for (int i = 0; i < datafile.sets.count; ++i) {
      free(datafile.sets.items[i].sizes);
      free(datafile.sets.items[i].vals);
    }
    nob_da_free(datafile.sets);
  }

  return 0;
}
