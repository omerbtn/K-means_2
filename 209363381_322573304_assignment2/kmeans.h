# ifndef KMEANS_H_
# define KMEANS_H_

double** k_means_actually(double** points, int* centroids_indices, int k, int n, int d, int iter, double eps);

# endif