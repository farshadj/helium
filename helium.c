#include "Python.h"
#include <he.h>

struct module_state {
	PyObject *error;
};

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
	printf("%s\n", "error_out");
	return NULL;
}

/**
 * The function opens Helium Datastore and returns HeliumHandler
 * Implementation of helium_open
 *
 * inputs: url, name and flags
 * output: heliumHandler
 *
 */
static PyObject *helium_open(PyObject *self, PyObject *args) {

	const char *url, *name;
	int flags;

	//TODO: env - the 4th input of helium open to be defined

	/* Parse the input tuple */
	if (!PyArg_ParseTuple(args, "zzi", &url, &name, &flags)) {
		PyErr_SetString(PyExc_TypeError, "parameter must be callable");
		he_perror("input parameters are incorrect");
		return NULL;
	}

	//printf("%s\n","opening the handler");

	/* Call the Helium C function to open handler. */
	he_t he = he_open(url, name, flags, NULL);

	if (!he) {
		he_perror("he_open");
		return NULL;
	}

	//printf("handler is open - pointer is %p\n", &he);

	/* Clean up	 */

	/* Build the output pyObject. handler (he_t type) is converted to PyObject*/
	PyObject *ret = Py_BuildValue("O", he);

	if (!ret) {
		Py_DECREF(ret);
		return NULL;
	}

	/* returning helium handler as PyObject*/
	return ret;

}

/**
 * The function closes Helium handler and returns the integer output
 * Implementation of helium_close
 *
 * inputs: heliumHandler
 * output: heliumErrorcode - 0 if succeeds
 *
 */

static PyObject *helium_close(PyObject *self, PyObject *args) {
	PyObject *pyHeliumHandler;

	/* Parse the input tuple */
	if (!PyArg_ParseTuple(args, "O", &pyHeliumHandler)) {
		PyErr_SetString(PyExc_TypeError, "parameter must be callable");
		//he_perror("input parameters are incorrect");
		return NULL;
	}

	/* convert envObj to he_t */
	he_t heliumHandler = (he_t) pyHeliumHandler;

	/* Call the Helium C function to close handler. */
	int retVal = he_close(heliumHandler);

	/* Clean up	 */
//		Py_DECREF(heliumHandler);
//		Py_DECREF(pyHeliumHandler);
	printf("handler is closed output code: %i\n", retVal);
	PyObject *ret = Py_BuildValue("i", retVal);

	return ret;

}

/**
 * The function inserts key, value into Helium
 * Implementation of helium_insert
 *
 * inputs: heliumHandler, key, value
 * output: heliumErrorcode - 0 if succeeds
 *
 */
static PyObject *helium_insert(PyObject *self, PyObject *args) {

	PyObject *key, *val, *encapsHeliumHandler;

	/* Parse the input tuple */
	if (!PyArg_ParseTuple(args, "OOO", &encapsHeliumHandler, &key, &val)) {
		printf("%s\n", "error in parsing input values ");

		PyErr_SetString(PyExc_TypeError, "parameter must be callable");
		he_perror("input parameters are incorrect");
		return NULL;
	}

	/* retrieving the helium handler */
	he_t he = (he_t) encapsHeliumHandler;



	struct he_item item;
	item.key = (void *) &key;
	item.val = (void *) &val;
	item.key_len = (size_t) PyObject_Size(key);
	item.val_len = (size_t) PyObject_Size(val);


	/* Call the Helium C function to insert key,value */
	int retVal = he_insert(he, &item);

	/* Clean up	 */
	Py_DECREF(key);
	Py_DECREF(val);


	/* Build the output tuple - returning the helium strerror code - 0 if succeeds */
	PyObject *ret = Py_BuildValue("i", retVal);
	return ret;

}


/**
 * The function commits and closes transaction
 * Implementation of he_commit
 *
 * inputs: heliumHandler
 * output: heliumErrorcode - 0 if succeeds
 *
 */
static PyObject *helium_commit(PyObject *self, PyObject *args) {

	PyObject*encapsHeliumHandler;

	//printf("%s\n","commit");

	/* Parse the input tuple */
	if (!PyArg_ParseTuple(args, "O", &encapsHeliumHandler)) {
		printf("%s\n", "error in parsing input values ");

		PyErr_SetString(PyExc_TypeError, "parameter must be callable");
		he_perror("input parameters are incorrect");
		return NULL;
	}

	/* retrieving heilum Handler */

	he_t he = (he_t) encapsHeliumHandler;

	int retVal = he_commit(he);
	//printf("Commit return %d\n",retVal);

	/* Build the output tuple */
	PyObject *ret = Py_BuildValue("i", retVal);
	return ret;

}

/*
 * helium methods are mapped to respective functions.
 *
 */
static PyMethodDef helium_methods[] = { { "error_out", (PyCFunction) error_out,
		METH_NOARGS, NULL }, { "open", helium_open, METH_VARARGS,
		"open the Helium handler" }, { "insert", helium_insert, METH_VARARGS,
		"insert key,value" },
		{ "commit", helium_commit, METH_VARARGS, "Commit" },
//	{"lookup",helium_lookup, METH_VARARGS, "Lookup key,value from Helium"},
		{ "close", helium_close, METH_VARARGS, "closes Helium handler" }, {
				NULL, NULL, 0, NULL } };

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

void inithelium(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
	PyObject *module = PyModule_Create(&moduledef);
#else
	PyObject *module = Py_InitModule("helium", helium_methods);
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

