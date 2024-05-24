import random
import time
from itertools import combinations

# Read the datasets from the file
def read_datasets(file_path):
    datasets = []
    with open(file_path, 'r') as file:
        dataset = {}
        for line in file:
            if line.startswith("dataset"):
                if dataset:
                    datasets.append(dataset)
                dataset = {'sizes': [], 'vals': []}
            elif line.startswith("sizes ="):
                sizes_str = line.strip().split('=')[1].strip(' {}')
                dataset['sizes'] = list(map(int, sizes_str.split(',')))
            elif line.startswith("vals ="):
                vals_str = line.strip().split('=')[1].strip(' {}')
                dataset['vals'] = list(map(int, vals_str.split(',')))
        if dataset:
            datasets.append(dataset)
    return datasets

def knapsack_brute_force(sizes, vals, capacity):
    n = len(sizes)
    max_value = 0
    best_combination = []
    count = 0
    
    for r in range(1, n + 1):
        for combination in combinations(range(n), r):
            total_size = sum(sizes[i] for i in combination)
            total_value = sum(vals[i] for i in combination)
            count += 1
            if total_size <= capacity and total_value > max_value:
                max_value = total_value
                best_combination = combination
    
    return best_combination, max_value, count

def knapsack_heuristic(sizes, vals, capacity):
    index_and_ratios = sorted([(i, vals[i] / sizes[i]) for i in range(len(sizes))], key=lambda x: x[1], reverse=True)
    
    total_size = 0
    total_value = 0
    selected_items = []
    for i, _ in index_and_ratios:
        if total_size + sizes[i] <= capacity:
            selected_items.append(i)
            total_size += sizes[i]
            total_value += vals[i]
    
    return selected_items, total_value

def main():
    file_path = 'plecak.txt'
    datasets = read_datasets(file_path)
    
    # Randomly select a dataset
    dataset_index = random.randint(0, len(datasets) - 1)
    dataset = datasets[dataset_index]
    sizes = dataset['sizes']
    vals = dataset['vals']
    
    capacity = 50
    
    # Brute force solution
    start_time = time.time()
    best_combination, max_value, count = knapsack_brute_force(sizes, vals, capacity)
    end_time = time.time()
    brute_force_time = end_time - start_time
    brute_force_solution = [(i+1, sizes[i], vals[i]) for i in best_combination]
    
    # Heuristic solution
    start_time = time.time()
    heuristic_combination, heuristic_value = knapsack_heuristic(sizes, vals, capacity)
    end_time = time.time()
    heuristic_time = end_time - start_time
    heuristic_solution = [(i+1, sizes[i], vals[i]) for i in heuristic_combination]
    
    # Output results
    print(f"Selected dataset: {dataset_index + 1}")
    print("Brute Force Solution:")
    print(f"Items: {brute_force_solution}")
    print(f"Total Value: {max_value}")
    print(f"Total Size: {sum(sizes[i] for i in best_combination)}")
    print(f"Count of checked combinations: {count}")
    print(f"Execution Time: {brute_force_time} seconds")
    
    print("\nHeuristic Solution:")
    print(f"Items: {heuristic_solution}")
    print(f"Total Value: {heuristic_value}")
    print(f"Total Size: {sum(sizes[i] for i in heuristic_combination)}")
    print(f"Execution Time: {heuristic_time} seconds")

if __name__ == "__main__":
    main()
