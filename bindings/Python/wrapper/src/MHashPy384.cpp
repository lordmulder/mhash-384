/* ---------------------------------------------------------------------------------------------- */
/* MHash-384 - Language bindings for Java                                                         */
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

static PyObject *MHash384_Create(PyObject *const self, PyObject *const args)
{
	return PyLong_FromVoidPtr(new mhash::MHash384());
}

static PyObject *MHash384_Update(PyObject *const self, PyObject *const args)
{
	PyObject *instance = NULL, *data = NULL;
	if (PyArg_UnpackTuple(args, "MHash384_Update", 2, 2, &instance, &data))
	{
		if (PyLong_Check(instance) && PyBytes_Check(data))
		{
			void *const inst_ptr = PyLong_AsVoidPtr(instance);
			if (inst_ptr)
			{
				reinterpret_cast<mhash::MHash384*>(inst_ptr)->update(reinterpret_cast<uint8_t*>(PyBytes_AsString(data)), PyBytes_Size(data));
				Py_RETURN_TRUE;
			}
		}
	}
	Py_RETURN_FALSE;
}

static PyObject *MHash384_Result(PyObject *const self, PyObject *const args)
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

static PyObject *MHash384_FreeUp(PyObject *const self, PyObject *const args)
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

/* ------------------------------------------------------------------------*/
/* MODULE INTERFACCE                                                       */
/* ------------------------------------------------------------------------*/

static PyMethodDef MHash384_Methods[] =
{
	{"create", MHash384_Create, METH_NOARGS,  "Create a new MHash384 instance and initialize it."},
	{"update", MHash384_Update, METH_VARARGS, "Process next N bytes of input data. Pass the MHash384 instance followed by a PyBytes object."},
	{"result", MHash384_Result, METH_VARARGS, "Return the final hash (digest) value."},
	{"freeup", MHash384_FreeUp, METH_VARARGS, "Destorys an existing instace and frees all its memory."},
	{NULL, NULL, 0, NULL}
};

static struct PyModuleDef MHash384_ModuleDef =
{
	PyModuleDef_HEAD_INIT, "MHash384", "", -1, MHash384_Methods
};

PyMODINIT_FUNC
PyInit_MHash384(void)
{
	return PyModule_Create(&MHash384_ModuleDef);
}

/*
JNIEXPORT jlong JNICALL Java_mhash_MHash384_00024Internals_createInstance(JNIEnv*, jclass)
{
	return reinterpret_cast<jlong>(new mhash::MHash384());
}

JNIEXPORT void JNICALL Java_mhash_MHash384_00024Internals_update(JNIEnv *const env, jclass, const jlong instance, const jbyteArray input, const jint offset, const jint len)
{
	jbyte *const input_ptr = env->GetByteArrayElements(input, NULL);
	reinterpret_cast<mhash::MHash384*>(instance)->update(reinterpret_cast<uint8_t*>(input_ptr + offset), len);
	env->ReleaseByteArrayElements(input, input_ptr, JNI_ABORT);
}

JNIEXPORT jbyteArray JNICALL Java_mhash_MHash384_00024Internals_finalize(JNIEnv *const env, jclass, const jlong instance)
{
	const jbyteArray result = env->NewByteArray(MHASH_384_LEN);
	jbyte *const result_ptr = env->GetByteArrayElements(result, NULL);
	reinterpret_cast<mhash::MHash384*>(instance)->finalize(reinterpret_cast<uint8_t*>(result_ptr));
	env->ReleaseByteArrayElements(result, result_ptr, 0);
	return result;
}

JNIEXPORT void JNICALL Java_mhash_MHash384_00024Internals_destroyInstance(JNIEnv*, jclass, const jlong instance)
{
	delete reinterpret_cast<mhash::MHash384*>(instance);
}

JNIEXPORT jintArray JNICALL Java_mhash_MHash384_00024Internals_getVersion(JNIEnv *const env, jclass)
{
	const jintArray version = env->NewIntArray(3);
	jint *const version_ptr = env->GetIntArrayElements(version, NULL);
	version_ptr[0] = (jint) MHASH_384_VERSION_MAJOR;
	version_ptr[1] = (jint) MHASH_384_VERSION_MINOR;
	version_ptr[2] = (jint) MHASH_384_VERSION_PATCH;
	env->ReleaseIntArrayElements(version, version_ptr, 0);
	return version;
}
*/
