/* ---------------------------------------------------------------------------------------------- */
/* MHash-384 - Language bindings for Delphi                                                       */
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

#ifdef __cplusplus
#define EXPORT_FUNC(NAME,TYPE,...) extern "C" __declspec(dllexport) TYPE NAME(__VA_ARGS__)
#else
#define EXPORT_FUNC(NAME,TYPE,...) __declspec(dllexport) TYPE NAME(__VA_ARGS__)
#endif

EXPORT_FUNC(mhash384_create, uintptr_t, void)
{
	return (uintptr_t) new mhash::MHash384();
}

EXPORT_FUNC(mhash384_update, void, const uintptr_t instance, uint8_t *const input, const size_t offset, const size_t len)
{
	reinterpret_cast<mhash::MHash384*>(instance)->update(input + offset, len);
}

EXPORT_FUNC(mhash384_finalize, void, const uintptr_t instance, uint8_t *const buffer)
{
	reinterpret_cast<mhash::MHash384*>(instance)->finalize(buffer);
}

EXPORT_FUNC(mhash384_cleanup, void, const uintptr_t instance)
{
	delete reinterpret_cast<mhash::MHash384*>(instance);
}

EXPORT_FUNC(mhash384_getver, void, uint32_t *const major, uint32_t *const minor, uint32_t *const patch)
{
	*major = MHASH_384_VERSION_MAJOR;
	*minor = MHASH_384_VERSION_MINOR;
	*patch = MHASH_384_VERSION_PATCH;
}
