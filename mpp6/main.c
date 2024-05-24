#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    int *sizes;
    int *vals;
    int length;
} Dataset;

Dataset* read_datasets(const char *file_path, int *num_datasets) {
    FILE *file = fopen(file_path, "r");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    Dataset *datasets = NULL;
    int capacity = 10;
    datasets = malloc(capacity * sizeof(Dataset));

    char line[256];
    int dataset_index = -1;
    int item_index = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "dataset") == line) {
            if (dataset_index >= 0) {
                datasets[dataset_index].length = item_index;
            }
            dataset_index++;
            item_index = 0;
            if (dataset_index >= capacity) {
                capacity *= 2;
                datasets = realloc(datasets, capacity * sizeof(Dataset));
            }
            datasets[dataset_index].sizes = malloc(100 * sizeof(int)); // Assuming max 100 items per dataset
            datasets[dataset_index].vals = malloc(100 * sizeof(int));
        } else if (strstr(line, "sizes =") == line) {
            char *sizes_str = strchr(line, '{') + 1;
            sizes_str[strlen(sizes_str) - 2] = '\0'; // Remove the closing bracket
            char *token = strtok(sizes_str, ",");
            while (token) {
                datasets[dataset_index].sizes[item_index] = atoi(token);
                token = strtok(NULL, ",");
                item_index++;
            }
        } else if (strstr(line, "vals =") == line) {
            char *vals_str = strchr(line, '{') + 1;
            vals_str[strlen(vals_str) - 2] = '\0'; // Remove the closing bracket
            char *token = strtok(vals_str, ",");
            item_index = 0;
            while (token) {
                datasets[dataset_index].vals[item_index] = atoi(token);
                token = strtok(NULL, ",");
                item_index++;
            }
            datasets[dataset_index].length = item_index;
        }
    }
    if (dataset_index >= 0) {
        datasets[dataset_index].length = item_index;
    }

    fclose(file);
    *num_datasets = dataset_index + 1;
    return datasets;
}

void knapsack_brute_force(int *sizes, int *vals, int n, int capacity, int *best_combination, int *max_value, int *count) {
    *max_value = 0;
    *count = 0;

    for (int r = 1; r < (1 << n); r++) {
        int total_size = 0;
        int total_value = 0;
        for (int i = 0; i < n; i++) {
            if (r & (1 << i)) {
                total_size += sizes[i];
                total_value += vals[i];
            }
        }
        (*count)++;
        if (total_size <= capacity && total_value > *max_value) {
            *max_value = total_value;
            memcpy(best_combination, &r, sizeof(int));
        }
    }
}

void knapsack_heuristic(int *sizes, int *vals, int n, int capacity, int *selected_items, int *total_value, int *selected_count) {
    float ratios[n];
    int indices[n];
    for (int i = 0; i < n; i++) {
        ratios[i] = (float)vals[i] / sizes[i];
        indices[i] = i;
    }

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (ratios[j] < ratios[j + 1]) {
                float temp_ratio = ratios[j];
                ratios[j] = ratios[j + 1];
                ratios[j + 1] = temp_ratio;

                int temp_index = indices[j];
                indices[j] = indices[j + 1];
                indices[j + 1] = temp_index;
            }
        }
    }

    *total_value = 0;
    int total_size = 0;
    *selected_count = 0;
    for (int i = 0; i < n; i++) {
        int index = indices[i];
        if (total_size + sizes[index] <= capacity) {
            selected_items[(*selected_count)++] = index;
            total_size += sizes[index];
            *total_value += vals[index];
        }
    }
}

int main() {
    srand(time(NULL));
    const char *file_path = "plecak.txt";
    int num_datasets;
    Dataset *datasets = read_datasets(file_path, &num_datasets);

    int dataset_index = rand() % num_datasets;
    Dataset dataset = datasets[dataset_index];
    int *sizes = dataset.sizes;
    int *vals = dataset.vals;
    int capacity = 50;

    // Brute force solution
    int best_combination = 0;
    int max_value;
    int count;
    clock_t start_time = clock();
    knapsack_brute_force(sizes, vals, dataset.length, capacity, &best_combination, &max_value, &count);
    clock_t end_time = clock();
    double brute_force_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("Selected dataset: %d\n", dataset_index + 1);
    printf("Brute Force Solution:\n");
    printf("Items: ");
    for (int i = 0; i < dataset.length; i++) {
        if (best_combination & (1 << i)) {
            printf("(%d, %d, %d) ", i + 1, sizes[i], vals[i]);
        }
    }
    printf("\nTotal Value: %d\n", max_value);
    printf("Total Size: ");
    int total_size = 0;
    for (int i = 0; i < dataset.length; i++) {
        if (best_combination & (1 << i)) {
            total_size += sizes[i];
        }
    }
    printf("%d\n", total_size);
    printf("Count of checked combinations: %d\n", count);
    printf("Execution Time: %f seconds\n", brute_force_time);

    // Heuristic solution
    int selected_items[dataset.length];
    int heuristic_value;
    int selected_count;
    start_time = clock();
    knapsack_heuristic(sizes, vals, dataset.length, capacity, selected_items, &heuristic_value, &selected_count);
    end_time = clock();
    double heuristic_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("\nHeuristic Solution:\n");
    printf("Items: ");
    for (int i = 0; i < selected_count; i++) {
        printf("(%d, %d, %d) ", selected_items[i] + 1, sizes[selected_items[i]], vals[selected_items[i]]);
    }
    printf("\nTotal Value: %d\n", heuristic_value);
    printf("Total Size: ");
    total_size = 0;
    for (int i = 0; i < selected_count; i++) {
        total_size += sizes[selected_items[i]];
    }
    printf("%d\n", total_size);
    printf("Execution Time: %f seconds\n", heuristic_time);

    for (int i = 0; i < num_datasets; i++) {
        free(datasets[i].sizes);
        free(datasets[i].vals);
    }
    free(datasets);

    return 0;
}
