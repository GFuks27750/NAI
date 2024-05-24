import numpy as np


class KMeans:
    def __init__(self, k=3):
        self.k = k
        self.centroids = None

    @staticmethod
    #statyczna metoda odpowiedzialna za obliczenie punktu w przestrzeni od centroidu
    def euclidian_distance(data_point, centroids):
        return np.sqrt(np.sum((centroids - data_point)**2, axis=1))
    
    def fit(self, X, max_iters=100):
        #Ten fragment kodu, zabezpiecza nas przed sytuacją w której punkt centroidu
        #został by wylosowany poza sensownymi granicami naszego rozmieszczenia punktów 
        #w przestrzeni. Przykładowo jeżeli maksymalna wartość y to 100, a x to 200,
        #to będziemy mieli pewność, że w tym przedziale zostanie wybrany centroid, a nie 
        #poza nim. 
        self.centroids = np.random.uniform(np.amin(X, axis=0),
                                           np.amax(X, axis=0),
                                           size=(self.k, X.shape[1]))
        
        for i in range(max_iters):
            #lista przechowująca informacje o tym, dla jakiego punktu w przestrzni
            #przypada jaki cluster
            y = []
            #print(f"{'Punkt':^10} | {'Klaster':^10} | {'Odległość':^20}")
            print("-" * 42)
            
            for index,data_point in enumerate(X):
                #distances to lista zawierająca wszystkie odległości (sumy kwadratów) punktów,
                #od swoich centroidów, następnie cluster_num pobiera najmniejszą odległość punktu 
                #od centroida i dodaje ją do listy y
                distances = KMeans.euclidian_distance(data_point, self.centroids)
                cluster_num = np.argmin(distances)
                y.append(cluster_num)
                
               # print(f"{index:^10} | {cluster_num:^10} | {distances[cluster_num]:^20.4f}")
            
            
            y = np.array(y)
            
            #tutaj sprawdzamy do jakich armii przypadają jakie punkty, tak żeby
            #przygotować się przed kolejną ewaluacją punktów
            cluster_indices = []

            for i in range(self.k):
                cluster_indices.append(np.argwhere(y==i))

            cluster_centers = []

            for i, indices in enumerate(cluster_indices):
                #jeżeli armia po przebiegu pozostała pusta, nie ma sensu ewaluować jego środka,
                #wtedy jego środek, pozostaje oryginalnym centroidem
                if len(indices) == 0:
                    cluster_centers.append(self.centroids[i])
                #w przeciwnym wypadku bierzemy średnią ze wszystkich obliczonych wartości, i 
                #pierwsza obliczona wartość z listy cluster_centers zostanie naszym nowym centroidem
                else:
                    cluster_centers.append(np.mean(X[indices], axis=0)[0])
            print(f"suma kwadratow odleglosci: {sum(distances**2)}")
            
            #jeżeli zmiana centroidów jest bardzo mała, albo w ogóle nie zaszła wśród 
            #wszystkich centroidów, nie ma sensu dalej przeprowadzać ewaluacji nowych środków,
            #w takiej sytuacji bierzemy jakąś bardzo małą wartość i sprawdzamy czy różnica 
            #naszch centroidów w stosunku do środków armii jest mniejsza niż ta wartość, 
            #to przerywamy pętle
            if np.max(self.centroids - np.array(cluster_centers)) < 0.0001:
                print(f"Finalna liczba iteracji: {i+1}")
                break
            #w przeciwnym wypadku repozycjonujemy centroidy, przypisując im środki armii
            else:
                self.centroids = np.array(cluster_centers)
        
        return y
    
