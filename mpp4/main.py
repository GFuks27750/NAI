from KMeans import KMeans
import matplotlib.pyplot as plt
import numpy as np


def import_data(fileName):
    data = []
    labels = []
    label_map = {'Iris-setosa': 0, 'Iris-versicolor': 1, 'Iris-virginica': 2}
    with open(fileName) as file:
        for line in file:
            parts = line.strip().split("\t")
            data.append([float(part) for part in parts[:-1]])
            labels.append(label_map[parts[-1].strip()])
    return np.array(data), np.array(labels)

def calculate_entropy(label_counts):
    total = sum(label_counts)
    if total == 0:
        return 0
    probabilities = label_counts / total
    return -np.sum([p * np.log2(p) for p in probabilities if p > 0])

# Załadowanie danych i etykiet
data, true_labels = import_data("iris_data.txt")

# Inicjalizacja i dopasowanie KMeans
k = int(input("Podaj k: "))
kmeans = KMeans(k)
predicted_labels = kmeans.fit(data)

# Obliczanie entropii dla każdego klastra
clusters = {i: [] for i in range(kmeans.k)}
for label, true_label in zip(predicted_labels, true_labels):
    clusters[label].append(true_label)

entropies = []
dominant_species = []
species_names = ['Iris-setosa', 'Iris-versicolor', 'Iris-virginica']
for cluster, labels in clusters.items():
    label_counts = np.bincount(labels, minlength=3)
    entropy_value = calculate_entropy(label_counts)
    entropies.append(entropy_value)
    dominant_species.append(species_names[np.argmax(label_counts)])
    print(f"Klaster {cluster}: entropia = {entropy_value:.4f}, Dominujący gatunek: {species_names[np.argmax(label_counts)]}")

# Wizualizacja wyników
def generate_random_colors(num_colors):
    return np.random.rand(num_colors, 3)

# Obliczanie entropii i znalezienie dominujących gatunków jak wcześniej

# Wizualizacja wyników z losowo wybranymi kolorami
plt.figure(figsize=(10, 6))
colors = generate_random_colors(kmeans.k)  # Generowanie losowych kolorów dla klastrów

for i in range(kmeans.k):
    cluster_data = data[predicted_labels == i]
    plt.scatter(cluster_data[:, 0], cluster_data[:, 1], color=colors[i], label=f'Klaster {i+1} ({dominant_species[i]})')

# Dodajemy centroidy
plt.scatter(kmeans.centroids[:, 0], kmeans.centroids[:, 1], c='red', s=200, marker='*', label='Centroidy')
for i, centroid in enumerate(kmeans.centroids):
    plt.text(centroid[0], centroid[1], f"{dominant_species[i]}", fontsize=12, ha='right')

plt.title('Klasteryzacja metodą K-Means z Iris dataset')
plt.legend(title="Klastry")
plt.show()

