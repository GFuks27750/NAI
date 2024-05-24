from Perceptron import Perceptron
import numpy as np
def load_and_prepare_data(filename):
    with open(filename, 'r') as file:
        lines = file.readlines()

    features = []
    labels = []

    for line in lines:
        values = line.strip().split('\t')
        features.append([float(value) for value in values[:-1]])
        labels.append(1 if values[-1].strip() == 'Iris-setosa' else 0)

    return np.array(features), np.array(labels)

def accuracy(y_true, y_pred):
    return np.sum(y_true == y_pred) / len(y_true)

# Load and prepare data
train_features, train_labels = load_and_prepare_data('iris_training 3.txt')
test_features, test_labels = load_and_prepare_data('iris_test 3.txt')

# Initialize and train Perceptron
perceptron = Perceptron(l_rate=0.01, n_iters=1000)
perceptron.fit(train_features, train_labels)

# Test predictions
predictions = perceptron.predict(test_features)
acc = accuracy(test_labels, predictions)
print(f"Correctly classified examples: {np.sum(test_labels == predictions)}")
print(f"Accuracy: {acc * 100}%")

# Function for manual input
def classify_manual_input(perceptron):
    while True:
        input_vector = input("Enter feature values separated by a comma, or 'q' to quit: ")
        if input_vector.strip().lower() == 'q':
            print("Exiting manual classification.")
            break
        try:
            input_features = np.array([float(value) for value in input_vector.split(',')])
            prediction = perceptron.predict(input_features.reshape(1, -1))
            print("Classified as:", "Iris-setosa" if prediction == 1 else "Not Iris-setosa")
        except ValueError:
            print("Invalid input, please enter numbers separated by commas.")

classify_manual_input(perceptron)
