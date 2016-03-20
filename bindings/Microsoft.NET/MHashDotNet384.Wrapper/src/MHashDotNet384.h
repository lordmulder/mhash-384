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

#pragma once

#include <stdlib.h>

using namespace System;
using namespace System::Runtime::InteropServices;
using System::Collections::Generic::IList;

namespace MHashDotNet {

	public ref class MHash384
	{
	public:
		MHash384(void);
		~MHash384(void);

		void Update(array<Byte> ^input);
		void Update(array<Byte> ^input, const Int64 offset, const Int64 count);
		void Update(IList<Byte> ^input);
		array<Byte>^ GetResult(void);

		static void Version([Out] UInt32 %major, [Out] UInt32 %minor, [Out] UInt32 %patch);

	private:
		const uintptr_t m_instance;
	};
}
