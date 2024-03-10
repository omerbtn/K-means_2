#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "kmeans.h"

#define     ITER_DEFAULT    200
#define     epsilon         0.001

double** k_means_actually(double**, int*, int, int, int, int, double);
int      findClosestCentroid(double*, double**, int, int);
void     updatePoints(double**, double**, int*, int, int, int);
int      updateCentroids(double**, int*, double**, int, int, int, double);
double   dist(double*, double*, int);
void     freeMemory(double**, int*, int, int);


double** k_means_actually(double** points, int* centroids_indices, int k, int n, int d, int iter, double eps) {
    double **centroids;
    int *cluster_index;
    int i, update; 

    centroids = calloc(k, sizeof(double*));
    cluster_index = calloc(n, sizeof(int));

    if (centroids == NULL || cluster_index == NULL){
        freeMemory(centroids, cluster_index, n, k);
        printf("An Error Has Occurred\n");
        return NULL;
    }

    for (i = 0 ; i < k ; i++) {
        centroids[i] = calloc(d, sizeof(double));
        if (centroids[i] == NULL){
            freeMemory(centroids, cluster_index, n, k);
            printf("An Error Has Occurred\n");
            return NULL;
        }
    }

    for (i = 0 ; i < k ; i++){
        memcpy(centroids[i], points[centroids_indices[i]], sizeof(double) * d);
    }

    for (i = 0 ; i < iter ; i++){
        updatePoints(points, centroids, cluster_index, n, k, d);

        update = updateCentroids(centroids, cluster_index, points, n, k, d, eps);
        if (update == 1){
            break;
        }
        if (update == -1){
            freeMemory(centroids, cluster_index, n, k);
            printf("An Error Has Occurred\n");   
            return NULL;
        }
    }
    
    freeMemory(NULL, cluster_index, n, k);   
    return centroids;
}


int updateCentroids(
    double** centroids, int* cluster_index, double** points, 
    int n, int k, int d, double eps){

    double* new_centroid;
    int converged, i, j, x, cnt;
    
    new_centroid = calloc(d, sizeof(double));
    converged = 1;

    if (new_centroid == NULL){
        return -1;
    }

    for (i = 0 ; i < k ; i++){
        for (x = 0; x < d; x++){
            new_centroid[x] = 0;
        }
        
        cnt = 0;
        for (j = 0 ; j < n ; j++){
            if (cluster_index[j] == i){
                for (x = 0 ; x < d ; x++){
                    new_centroid[x] += points[j][x];
                }
                cnt++;
            }
        }

        for (x = 0 ; x < d ; x++){
            new_centroid[x] /= cnt; 
        }

        if (dist(centroids[i], new_centroid, d) >= eps){
            converged = 0;
        }

        memcpy(centroids[i], new_centroid, sizeof(double) * d);
    }

    free(new_centroid);

    return converged;
}


int findClosestCentroid(double* point, double** centroids, int k, int d){
    double minDist, currDist;
    int minDistIndex;
    int i;

    minDist = dist(point, centroids[0], d);
    minDistIndex = 0;
    for (i = 1; i < k; i++){
        currDist = dist(point, centroids[i], d);
        if (currDist < minDist){
            minDist = currDist;
            minDistIndex = i;
        }
    }
    return minDistIndex;
}

void updatePoints(
    double** points, double** centroids, int* cluster_index, int n, int k, int d){
    int j;
    
    for (j = 0 ; j < n ; j++){
        cluster_index[j] = findClosestCentroid(points[j], centroids, k, d);
    }
}

double dist(double* p1, double* p2, int d){
    double diff, sum = 0;
    int i;

    for (i = 0; i < d; i++){
        diff = p1[i] - p2[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

void freeMemory(double** centroids, int* cluster_index, int n, int k){
    int i;

    if (centroids != NULL){
        for (i = 0; i < k; i++){
            free(centroids[i]);
        }
        free(centroids);
    }

    free(cluster_index);
}