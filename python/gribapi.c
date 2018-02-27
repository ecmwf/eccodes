/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "Python.h"
#include "grib_api.h"

static PyObject* Error;

PyDoc_STRVAR(gribapi__doc__,
		"gribapi point evalutation kernel");

PyDoc_STRVAR(read__doc__,
		"gribapi point evalutation kernel");

static PyObject *
py_new_handle_from_file(PyObject *self, PyObject *args)
{
	/* TODO: Call PyFile_Check or PyFile_CheckExact */
	int err = 0;
	grib_handle *h;
	PyObject *file;

	if(!PyArg_ParseTuple(args,"O",&file))
		return NULL;

	h = grib_handle_new_from_file(NULL,PyFile_AsFile(file),&err);

	if(err)  {
		/* http://www.ragestorm.net/tutorial?id=21 */
		PyErr_Format(Error,"grib_api: %s",grib_get_error_message(err));
		return NULL;
	}

	
	/* TODO: Do an OO version */

	if(h)
	return PyLong_FromVoidPtr(h);

	Py_RETURN_NONE;
}

static PyObject *
py_handle_delete(PyObject *self, PyObject *args)
{
	PyObject* h;
	if(!PyArg_ParseTuple(args,"O",&h))	
		return NULL;
	grib_handle_delete(PyLong_AsVoidPtr(h));
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *
py_keys_iterator_delete(PyObject *self, PyObject *args)
{
	PyObject* h;
	if(!PyArg_ParseTuple(args,"O",&h))	
		return NULL;
	grib_keys_iterator_delete(PyLong_AsVoidPtr(h));
	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject *
py_get_string(PyObject *self, PyObject *args)
{
	PyObject* h;
	char *s;
	size_t size;
	char tmp[1024];
	int err;
	
	if(!PyArg_ParseTuple(args,"Os",&h,&s))	
		return NULL;

	size = sizeof(tmp);
	err  = grib_get_string(PyLong_AsVoidPtr(h),s,tmp,&size);

	if(err)  {
		/* http://www.ragestorm.net/tutorial?id=21 */
		PyErr_Format(Error,"grib_api: %s %s",s,grib_get_error_message(err));
		return NULL;
	}

	return PyString_InternFromString(tmp);

}

static PyObject *
py_keys_iterator_next(PyObject *self, PyObject *args)
{
    PyObject* h;
	int more;

    if(!PyArg_ParseTuple(args,"O",&h))
        return NULL;

	more = grib_keys_iterator_next(PyLong_AsVoidPtr(h));
	if(more)
	{
		Py_RETURN_TRUE;
	}
	else
	{
		Py_RETURN_FALSE;
	}
}


static PyObject *
py_keys_iterator_get_string(PyObject *self, PyObject *args)
{
	PyObject* h;
	size_t size;
	char tmp[1024];
	int err;
	
	if(!PyArg_ParseTuple(args,"O",&h))	
		return NULL;

	size = sizeof(tmp);
	err  = grib_keys_iterator_get_string(PyLong_AsVoidPtr(h),tmp,&size);

	if(err)  {
		/* http://www.ragestorm.net/tutorial?id=21 */
		PyErr_Format(Error,"grib_api: %s",grib_get_error_message(err));
		return NULL;
	}

	return PyString_InternFromString(tmp);

}

static PyObject *
py_keys_iterator_get_name(PyObject *self, PyObject *args)
{
	PyObject* h;
	
	if(!PyArg_ParseTuple(args,"O",&h))	
		return NULL;


	return PyString_InternFromString(grib_keys_iterator_get_name(PyLong_AsVoidPtr(h)));

}

static PyObject *
py_keys_iterator_new(PyObject *self, PyObject *args)
{
	PyObject* h;
	char *s;

	if(!PyArg_ParseTuple(args,"Os",&h,&s))	
		return NULL;

	return PyLong_FromVoidPtr(grib_keys_iterator_new(PyLong_AsVoidPtr(h),GRIB_KEYS_ITERATOR_ALL_KEYS,s));
}

static PyMethodDef gribapi_methods[] = {

	{"new_handle_from_file",  py_new_handle_from_file, METH_VARARGS, read__doc__},
	{"get_string",            py_get_string, METH_VARARGS, read__doc__},
	{"keys_iterator_new",     py_keys_iterator_new, METH_VARARGS, read__doc__},
	{"keys_iterator_delete",  py_keys_iterator_delete, METH_VARARGS, read__doc__},
	{"handle_delete",         py_handle_delete, METH_VARARGS, read__doc__},
	{"keys_iterator_get_string",         py_keys_iterator_get_string, METH_VARARGS, read__doc__},
	{"keys_iterator_get_name",         py_keys_iterator_get_name, METH_VARARGS, read__doc__},
	{"keys_iterator_next",         py_keys_iterator_next, METH_VARARGS, read__doc__},


	{NULL, NULL}      /* sentinel */
};

PyMODINIT_FUNC
initgribapi(void)
{
	PyObject* m;

	/* There have been several InitModule functions over time */
	m = Py_InitModule3("gribapi", gribapi_methods, gribapi__doc__); 

	/* TODO: Create our own exception */
	Error = PyErr_NewException("grib.apierror",NULL,NULL);
	Py_INCREF(Error);
	PyModule_AddObject(m, "error", Error);
}

