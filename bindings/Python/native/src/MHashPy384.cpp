/* ---------------------------------------------------------------------------------------------- */
/* MHash-384 - Language bindings for Python                                                       */
/* Copyright(c) 2016 LoRd_MuldeR <mulder2@gmx.de>                                                 */
/*                                                                                                */
/* Permission is hereby granted, free of charge, to any person obtaining a copy of this software  */
/* and associated documentation files (the "Software"), to deal in the Software without           */
/* restriction, including without limitation the rights to use, copy, modify, merge, publish,     */
/* distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the  */
/* Software is furnished to do so, subject to the following conditions:                           */
/*                                                                                                */
/* The above copyright notice and this permission notice shall be included in all copies or       */
/* substantial portions of the Software.                                                          */
/*                                                                                                */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING  */
/* BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND     */
/* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   */
/* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.        */
/* ---------------------------------------------------------------------------------------------- */

#define MHASH_DISABLE_STL 1

#include <mhash_384.h>
#include <Python.h>

/* ------------------------------------------------------------------------*/
/* METHOD IMPLEMENTATION                                                   */
/* ------------------------------------------------------------------------*/

static PyObject *MHashPy384_Create(PyObject *const self, PyObject *const args)
{
	return PyLong_FromVoidPtr(new mhash::MHash384());
}

static PyObject *MHashPy384_Update(PyObject *const self, PyObject *const args)
{
	PyObject *instance = NULL, *data = NULL, *offset = NULL, *len = NULL;
	if (PyArg_UnpackTuple(args, "MHash384_Update", 2, 4, &instance, &data, &offset, &len))
	{
		if (PyLong_Check(instance) && PyBytes_Check(data))
		{
			void *const inst_ptr = PyLong_AsVoidPtr(instance);
			if (inst_ptr)
			{
				const size_t total_size = PyBytes_Size(data);
				const size_t offset_val = (offset && PyLong_Check(offset)) ? PyLong_AsSize_t(offset) : 0U;
				if (offset_val < total_size)
				{
					const size_t len_val = (len && PyLong_Check(len)) ? PyLong_AsSize_t(len) : (total_size - offset_val);
					const size_t sum = offset_val + len_val;
					if ((sum >= offset_val) && (sum >= len_val) && (sum <= total_size))
					{
						reinterpret_cast<mhash::MHash384*>(inst_ptr)->update(reinterpret_cast<uint8_t*>(PyBytes_AsString(data)) + offset_val, len_val);
						Py_RETURN_TRUE;
					}
				}
			}
		}
	}
	Py_RETURN_FALSE;
}

static PyObject *MHashPy384_Result(PyObject *const self, PyObject *const args)
{
	uint8_t buffer[MHASH_384_LEN];
	PyObject *instance = NULL;
	if (PyArg_UnpackTuple(args, "MHash384_Update", 1, 1, &instance))
	{
		if (PyLong_Check(instance))
		{
			void *const inst_ptr = PyLong_AsVoidPtr(instance);
			if (inst_ptr)
			{
				reinterpret_cast<mhash::MHash384*>(inst_ptr)->finalize(buffer);
				return PyByteArray_FromStringAndSize(reinterpret_cast<const char*>(buffer), MHASH_384_LEN);
			}
		}
	}
	Py_RETURN_NONE;
}

static PyObject *MHashPy384_FreeUp(PyObject *const self, PyObject *const args)
{
	PyObject *instance = NULL;
	if (PyArg_UnpackTuple(args, "MHash384_Update", 1, 1, &instance))
	{
		if (PyLong_Check(instance))
		{
			void *const inst_ptr = PyLong_AsVoidPtr(instance);
			if (inst_ptr)
			{
				delete reinterpret_cast<mhash::MHash384*>(inst_ptr);
				Py_RETURN_TRUE;
			}
		}
	}
	Py_RETURN_FALSE;
}

static PyObject *MHashPy384_GetVer(PyObject *const self, PyObject *const args)
{
	return Py_BuildValue("(kkk)", MHASH_384_VERSION_MAJOR, MHASH_384_VERSION_MINOR, MHASH_384_VERSION_PATCH);
}

/* ------------------------------------------------------------------------*/
/* MODULE INTERFACCE                                                       */
/* ------------------------------------------------------------------------*/

static PyMethodDef MHashPy384_Methods[] =
{
	{"create", MHashPy384_Create, METH_NOARGS,  "Create a new MHash384 instance and initialize it."},
	{"update", MHashPy384_Update, METH_VARARGS, "Process next N bytes of input data. Pass the MHash384 instance followed by a PyBytes object."},
	{"result", MHashPy384_Result, METH_VARARGS, "Return the final hash (digest) value."},
	{"freeup", MHashPy384_FreeUp, METH_VARARGS, "Destorys an existing instace and frees all its memory."},
	{"getver", MHashPy384_GetVer, METH_NOARGS,  "Get the library version, returned as a tuple of (Major, Minor, Patch)."},
	{NULL, NULL, 0, NULL}
};

static struct PyModuleDef MHash384_ModuleDef =
{
	PyModuleDef_HEAD_INIT, "MHashPy384_Native", "", -1, MHashPy384_Methods
};

PyMODINIT_FUNC
PyInit_MHashPy384_Native(void)
{
	return PyModule_Create(&MHash384_ModuleDef);
}
