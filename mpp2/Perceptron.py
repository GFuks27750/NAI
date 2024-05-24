import numpy as np

class Perceptron:
    def __init__(self, l_rate=0.01, n_iters=1000):
        # l_rate: szybkość uczenia (learning rate), określa krok aktualizacji wag
        # n_iters: liczba iteracji algorytmu na danych uczących
        self.l_rate = l_rate
        self.n_iters = n_iters
        self.weights = None  # wagi modelu, które będą uczono
        self.activation_function = self._unit_step_func  # funkcja aktywacji (tutaj: funkcja skokowa)

    def _unit_step_func(self, x):
        # Funkcja aktywacji: funkcja skokowa, zwraca 1 dla x >= 0, w przeciwnym wypadku 0
        return np.where(x >= 0, 1, 0)

    def fit(self, X, y):
        # Metoda służąca do trenowania perceptronu
        n_samples, n_features = X.shape  # Liczba próbek i cech w danych wejściowych
        # Dodajemy kolumnę jedynkową do X, reprezentującą wyraz wolny (bias)
        X = np.c_[X, np.ones((n_samples))]
        # Inicjalizacja wag, w tym wagi dla wyrazu wolnego
        self.weights = np.zeros(n_features + 1)

        for _ in range(self.n_iters):
            for idx, x_i in enumerate(X):
                linear_output = np.dot(x_i, self.weights)  # Obliczanie wyjścia liniowego modelu
                y_predicted = self.activation_function(linear_output)  # Predykcja za pomocą funkcji aktywacji
                # Aktualizacja wag na podstawie reguły delty (błąd * szybkość uczenia * wejście)
                update = self.l_rate * (y[idx] - y_predicted)
                self.weights += update * x_i

    def predict(self, X):
        # Metoda służąca do dokonywania predykcji na nowych danych
        # Dodanie kolumny jedynkowej do danych wejściowych
        X = np.c_[X, np.ones((X.shape[0]))]
        # Obliczenie wyjścia liniowego
        linear_output = np.dot(X, self.weights)
        # Predykcja za pomocą funkcji aktywacji
        y_predicted = self.activation_function(linear_output)
        return y_predicted
