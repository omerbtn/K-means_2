#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <math.h>
#include "kmeans.h"


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
    double d;

    if(!PyArg_ParseTuple(args, "iiiidOO", &k, &n, &d, &iter, &eps, &pypoints, &pycentroids)){
        return NULL;
    }

    if (PyObject_Length(pypoints) < 0 || PyObject_Length(pycentroids) < 0){
        return NULL;
    }

    centroids_indices = calloc(k, sizeof(int));
    if (centroids_indices == NULL){
        // allocation error
        return NULL;
    }

    for (i = 0; i < k; i++){
        item = PyList_GetItem(pycentroids, i);
        index = (int) item;
        // index = python int to c int convertion
        centroids_indices[i] = index;
    }

    points = calloc(n, sizeof(double*));
    if (points == NULL){
        //allocation error, FREE MEMORY
        return NULL;
    }
    for (i = 0; i < n; i++){
        points[i] = calloc(d, sizeof(double));
        if (points[i] == NULL){
            // allocation error, FREE MEMORY
            return NULL;
        }
    }
    for (i = 0; i < n; i++){
        point = PyList_GetItem(points, i);
        for (j = 0; j < d; j++){
            item = PyList_GetItem(point, j);
            d = (double) item;
            // convert float to double
            points[i][j] = d;
        }
    }
    centroids = k_means_actually(points, centroids_indices, k, n, d, iter, eps);
    // FREE ALL MEMORY ALLOCATED

    
    return Py_BuildValue("O", centroids);
}


static PyMethodDef kmeansMethods[] = {
    {"fit",
    (PyCFunction) fit,
    METH_VARARGS,
    PyDoc_STR("kmeans")},
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
