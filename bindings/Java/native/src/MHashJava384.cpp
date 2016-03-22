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
#include <mhash_384.jni.h>

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
