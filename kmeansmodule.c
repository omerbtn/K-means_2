#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <math.h>
#include "kmeans.h"

// double** k_means_actually(double**, int*, int, int, int, int, double);

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
    centroids_indices = calloc(k, sizeof(int));
    if (centroids_indices == NULL){
        // allocation error
        return NULL;
    }

    for (i = 0; i < k; i++){
        item = PyList_GetItem(pycentroids, i);
        index = PyLong_AsLong(item); // Supposed to be ok and translate to int, and k will not be too large...
        centroids_indices[i] = index;
    }
    
    // create C empty points array of n points
    points = calloc(n, sizeof(double*));
    if (points == NULL){
        //allocation error
        //FREE MEMORY
        return NULL;
    }
    // for each point allocate memory
    for (i = 0; i < n; i++){
        points[i] = calloc(d, sizeof(double));
        if (points[i] == NULL){
            // allocation error
            //FREE MEMORY
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
    // FREE ALL MEMORY ALLOCATED

    // NEED TO WORK THIS OUT
    return Py_BuildValue("O", centroids);
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
