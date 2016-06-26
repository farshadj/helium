#include "Python.h"
#include <he.h>

typedef struct {
  PyObject_HEAD
  long value;
} he_item;



struct module_state {
    PyObject *error;
};


static PyObject *helium_open(PyObject *self, PyObject *args);

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct module_state _state;
#endif

static PyObject *
error_out(PyObject *m) {
    struct module_state *st = GETSTATE(m);
    PyErr_SetString(st->error, "something bad happened");
    return NULL;
}

static PyMethodDef helium_methods[] = {
    {"open",helium_open, METH_VARARGS, "This function to open the handler"},
    {NULL, NULL,0,NULL}
};

#if PY_MAJOR_VERSION >= 3

static int helium_traverse(PyObject *m, visitproc visit, void *arg) {
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int helium_clear(PyObject *m) {
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}


static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "helium",
        NULL,
        sizeof(struct module_state),
        helium_methods,
        NULL,
        helium_traverse,
        helium_clear,
        NULL
};

#define INITERROR return NULL

PyMODINIT_FUNC
PyInit_helium(void)

#else
#define INITERROR return

void
inithelium(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *module = PyModule_Create(&moduledef);
#else
    PyObject *module = Py_InitModule3("helium", helium_methods,"Module description");
#endif


    if (module == NULL)
        INITERROR;
    struct module_state *st = GETSTATE(module);

    st->error = PyErr_NewException("helium.Error", NULL, NULL);
    if (st->error == NULL) {
        Py_DECREF(module);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return module;
#endif
}


static PyObject *helium_open(PyObject *self, PyObject *args)
{
    const char *url, *name, *tmp, *tmp2;
    int *flags;
    struct he_env *env;
    PyObject *envObj;

    he_t he;

    printf("%s\n","opening the handler");

    /* Parse the input tuple */
   if (!PyArg_ParseTuple(args, "ssdO", &url,&name,&flags,&env)){
	PyErr_SetString(PyExc_TypeError, "parameter must be callable");
        he_perror("input parameters are incorrect");
	return NULL;
   }
 
    printf("%s\n","opening the handler");

    /* Call the Helium C function to open handler. */
    he = he_open(url,name,flags,env);
    if (!he) {
         he_perror("he_open");
         return -1;
    }    

    printf("%s\n","handler is open");

    /* Build the output tuple */
    PyObject *ret = Py_BuildValue("O", he);
    return ret;
}






