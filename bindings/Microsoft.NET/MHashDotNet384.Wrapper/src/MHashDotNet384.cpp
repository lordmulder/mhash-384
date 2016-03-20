/* ---------------------------------------------------------------------------------------------- */
/* MHash-384 - Language bindings for Microsoft.NET                                                */
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

#include "MHashDotNet384.h"
#include <mhash_384.h>

#define M_INSATNCE ((mhash::MHash384*)m_instance)

MHashDotNet::MHash384::MHash384(void)
:
	m_instance((uintptr_t) new mhash::MHash384())
{
}

MHashDotNet::MHash384::~MHash384(void)
{
	delete (mhash::MHash384*) m_instance;
}

void MHashDotNet::MHash384::Update(array<Byte> ^input, const Int64 offset, const Int64 count)
{
#if SIZE_MAX < INT64_MAX
	if (input->LongLength > Int64(SIZE_MAX))
	{
		throw gcnew IndexOutOfRangeException(L"array length exceeds SIZE_MAX limit!");
	}
#endif
	if ((offset < 0) || (count < 0))
	{
		throw gcnew IndexOutOfRangeException(L"offset and/or count is negative!");
	}
	if ((offset + count > input->LongLength) || (Int64(offset + count) < Math::Max(offset, count)))
	{
		throw gcnew IndexOutOfRangeException(L"offset + count exceeds array bounds!");
	}
	pin_ptr<Byte> input_ptr = &input[0];
	M_INSATNCE->update(input_ptr + offset, size_t(count));
}

void MHashDotNet::MHash384::Update(array<Byte>^ input)
{
	Update(input, 0, input->LongLength);
}

void MHashDotNet::MHash384::Update(IList<Byte> ^input)
{
	array<Byte>^ buffer = gcnew array<Byte>(input->Count);
	input->CopyTo(buffer, 0);
	pin_ptr<Byte> buffer_ptr = &buffer[0];
	M_INSATNCE->update(buffer_ptr, (size_t)buffer->Length);
}

array<Byte>^ MHashDotNet::MHash384::GetResult(void)
{
	array<Byte>^ result = gcnew array<Byte>(MHASH_384_LEN);
	pin_ptr<Byte> result_ptr = &result[0];
	M_INSATNCE->finalize(result_ptr);
	return result;
}

void MHashDotNet::MHash384::Version([Out] UInt32 %major, [Out] UInt32 %minor, [Out] UInt32 %patch)
{
	major = MHASH_384_VERSION_MAJOR;
	minor = MHASH_384_VERSION_MINOR;
	patch = MHASH_384_VERSION_PATCH;
}
