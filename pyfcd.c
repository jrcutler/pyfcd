#include <Python.h>
#include <structmember.h>
#include <fcd.h>


typedef struct {
	PyObject_HEAD
	FCD *dev;
} PyFCD;


static PyObject *
PyFCD_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	PyFCD *self;

	self = (PyFCD *)type->tp_alloc(type, 0);
	if (self != NULL)
	{
		self->dev = NULL;
	}

	return (PyObject *)self;
}


static void
PyFCD_dealloc(PyFCD* self)
{
	fcd_close(self->dev);
	self->ob_type->tp_free((PyObject*)self);
}


static int
PyFCD_init(PyFCD *self, PyObject *args, PyObject *kwds)
{
	static char *kwlist[] = {"path", NULL};
	const char *path = NULL;

	if (! PyArg_ParseTupleAndKeywords(args, kwds, "|z", kwlist, &path))
		return -1; 

	/* open */
	self->dev = fcd_open(path);
	if (!self->dev)
	{
		PyErr_SetString(PyExc_TypeError, "Could not open FCD device");
		return -1;
	}

	return 0;
}


static PyObject *
PyFCD_get_frequency(PyFCD *self, void *closure)
{
	unsigned int freq;
	if (fcd_get_frequency_Hz(self->dev, &freq))
	{
		PyErr_SetFromErrno(PyExc_IOError);
		return NULL;
	}
	return PyInt_FromLong(freq);
}


static int
PyFCD_set_frequency(PyFCD *self, PyObject *value, void *closure)
{
	unsigned int freq;
	if (NULL == value)
	{
		PyErr_SetString(PyExc_TypeError, "Cannot delete frequency");
		return -1;
	}
	if (!PyInt_Check(value))
	{
		PyErr_SetString(PyExc_TypeError, "Frequency must be an integer");
		return -1;
	}

	freq = PyInt_AsUnsignedLongMask(value);

	if (fcd_set_frequency_Hz(self->dev, freq))
	{
		PyErr_SetFromErrno(PyExc_IOError);
		return -1;
	}


	return 0;
}


static PyGetSetDef PyFCD_getset[] = {
	{"frequency", (getter)PyFCD_get_frequency, (setter)PyFCD_set_frequency,
		"tuned frequency", NULL},
	{NULL} /* Sentinel */
};



static PyTypeObject PyFCDType = {
	PyObject_HEAD_INIT(NULL)
	0,                         /* ob_size */
	"pyfcd.FCD",               /* tp_name */
	sizeof(PyFCD),             /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)PyFCD_dealloc, /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_compare */
	0,                         /* tp_repr */
	0,                         /* tp_as_number */
	0,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash */
	0,                         /* tp_call */
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"FUNcube dongle object",   /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	0,                         /* tp_methods */
	0,                         /* tp_members */
	PyFCD_getset,              /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)PyFCD_init,      /* tp_init */
	0,                         /* tp_alloc */
	PyFCD_new,                 /* tp_new */
};


static PyMethodDef module_methods[] = {
	{NULL}  /* Sentinel */
};


#ifndef PyMODINIT_FUNC    /* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
initpyfcd(void) 
{
	PyObject* m;

	if (PyType_Ready(&PyFCDType) < 0)
		return;

	m = Py_InitModule3("pyfcd", module_methods,
		"Python FUNcube dongle interface.");

	if (m == NULL)
		return;

	Py_INCREF(&PyFCDType);
	PyModule_AddObject(m, "FCD", (PyObject *)&PyFCDType);
}
