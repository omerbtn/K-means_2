#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <math.h>
#include "kmeans.h"

void freeMemoryModule(double**, int*, double**, int, int);

static PyObject* fit(PyObject *self, PyObject *args){
    int k, n, d, iter, i, j;
    double eps;
    double** points;
    double** centroids;
    int* centroids_indices;
    PyObject *pypoints;
    PyObject *point;
    PyObject *pycentroids;
    PyObject *item;
    int index;
    double coordinate;

    if(!PyArg_ParseTuple(args, "OOiiiid", &pypoints, &pycentroids, &k, &n, &d, &iter, &eps)){
        return NULL;
    }

    if (PyObject_Length(pypoints) < 0 || PyObject_Length(pycentroids) < 0){
        return NULL;
    }

    // create C empty array of k indexes
    centroids_indices = malloc(k * sizeof(int));
    if (centroids_indices == NULL){
        // allocation error
        return NULL;
    }

    for (i = 0; i < k; i++){
        item = PyList_GetItem(pycentroids, i);
        index = (int) PyLong_AsLong(item); // Supposed to be ok and translate to int, and k will not be too large...
        centroids_indices[i] = index;
    }
    
    // create C empty points array of n points
    points = malloc(n * sizeof(double*));
    if (points == NULL){
        //allocation error
        //FREE MEMORY
        freeMemoryModule(points, centroids_indices, NULL, n, k);
        return NULL;
    }
    // for each point allocate memory
    for (i = 0; i < n; i++){
        points[i] = malloc(d * sizeof(double));
        if (points[i] == NULL){
            // allocation error
            //FREE MEMORY
            freeMemoryModule(points, centroids_indices, NULL, n, k);
            return NULL;
        }
    }
    // for each point create it from python in c
    for (i = 0; i < n; i++){ // going by each point
        point = PyList_GetItem(pypoints, i);
        for (j = 0; j < d; j++){
            coordinate = PyFloat_AsDouble(PyList_GetItem(point, j)); // Should work, since python's "points" array is in floats
            points[i][j] = coordinate;
        }
    }
    centroids = k_means_actually(points, centroids_indices, k, n, d, iter, eps);
    if (centroids == NULL){
        freeMemoryModule(points, centroids_indices, centroids, n, k);
        return NULL;
    }

    pycentroids = PyList_New(k); // Create the outer list
    if (pycentroids == NULL) {
        // Error handling
        freeMemoryModule(points, centroids_indices, centroids, n, k);
        return NULL;
    }

    // Iterate over rows of the array
    for (i = 0; i < k; i++) {
        PyObject *centroid_coords = PyList_New(d); // Create the inner list for each row
        if (centroid_coords == NULL) {
            // Error handling
            // FREE MEMORY
            // Py_DECREF(pycentroids);
            freeMemoryModule(points, centroids_indices, centroids, n, k);
            return NULL;
        }

        // Iterate over columns of the array
        for (j = 0; j < d; j++) {
            PyObject *value = PyFloat_FromDouble(centroids[i][j]); // Create a Python float object for each element
            if (value == NULL) {
                // Error handling
                // Py_DECREF(centroid_coords);
                // Py_DECREF(pycentroids);
                freeMemoryModule(points, centroids_indices, centroids, n, k);
                return NULL;
            }
            PyList_SET_ITEM(centroid_coords, j, value); // Set the element in the inner list
        }

        PyList_SET_ITEM(pycentroids, i, centroid_coords); // Set the inner list in the outer list
    }

    freeMemoryModule(points, centroids_indices, centroids, n, k);

    return pycentroids;

}


static PyMethodDef kmeansMethods[] = {
    {"fit",
    (PyCFunction) fit,
    METH_VARARGS,
    PyDoc_STR("Receives arguments and returns the appropriate centroids based on K-means algorithm")},
    {NULL, NULL, 0, NULL}
};


static struct PyModuleDef kmeansmodule = {
    PyModuleDef_HEAD_INIT,
    "mykmeanssp",
    NULL,
    -1,
    kmeansMethods
};


PyMODINIT_FUNC PyInit_mykmeanssp(void){
    PyObject *m;
    m = PyModule_Create(&kmeansmodule);
    if (!m){
        return NULL;
    }
    return m;
}


void freeMemoryModule(double** points, int* centroid_indices, double** centroids, int n, int k){
    int i;

    if (points != NULL){
        for (i = 0; i < n; i++){
            free(points[i]);
        }
        free(points);
    }

    free(centroid_indices);

    if (centroids != NULL){
        for (i = 0; i < k; i++){
            free(centroids[i]);
        }
        free(centroids);
    }
}