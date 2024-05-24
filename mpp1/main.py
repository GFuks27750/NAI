import math

# Funkcja do wczytywania danych z pliku
def load_data(file_path):
    data = []
    with open(file_path, 'r') as file:
        for line in file:
            parts = line.strip().split('\t')
            attributes = [float(x) for x in parts[:-1]]
            label = parts[-1].strip()
            data.append(attributes + [label])
    return data

# Funkcja do obliczania odległości Euklidesowej między dwoma wektorami7
def euclidean_distance(v1, v2):
    return math.sqrt(sum((v1[i] - v2[i]) ** 2 for i in range(len(v1))))


# Funkcja do klasyfikacji pojedynczego przykładu za pomocą algorytmu k-NN
def k_nn_classification(training_data, test_example, k):
    distances = [(data_point, euclidean_distance(data_point[:-1], test_example[:-1])) for data_point in training_data]
    distances.sort(key=lambda x: x[1])
    neighbors = distances[:k]

    votes = {}
    for neighbor, _ in neighbors:
        label = neighbor[-1]
        votes[label] = votes.get(label, 0) + 1

    return max(votes, key=votes.get)

# Funkcja do oceny dokładności klasyfikacji
def evaluate_accuracy(training_data, test_data, k):
    correct_predictions = sum(1 for test_example in test_data if k_nn_classification(training_data, test_example, k) == test_example[-1])
    accuracy = (correct_predictions / len(test_data)) * 100
    return correct_predictions, accuracy

def main():
    training_data = load_data("iris_training.txt")
    test_data = load_data("iris_test.txt")

    k = int(input("Podaj wartość k dla algorytmu k-NN: "))

    correct_predictions, accuracy = evaluate_accuracy(training_data, test_data, k)
    print(f"Liczba poprawnych klasyfikacji: {correct_predictions}, Dokładność: {accuracy}%")

    while True:
        user_input = input("Wprowadź wektor atrybutów do klasyfikacji (oddzielone przecinkiem) lub 'q' aby zakończyć: ").strip().lower()
        if user_input == 'q':
            break
        try:
            attributes = [float(x) for x in user_input.split(',')]
            prediction = k_nn_classification(training_data, attributes + [''], k)
            print(f"Przewidziana klasa dla wprowadzonego wektora: {prediction}")
        except ValueError:
            print("Błąd: niepoprawny format wektora. Spróbuj ponownie.")
        except IndexError:
            print("Podano za mało argumentów")

if __name__ == "__main__":
    main()
