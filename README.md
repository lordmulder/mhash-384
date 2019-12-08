% ![](img/mhash384/MHash-384.jpg)  
MHash-384
% Simple fast portable secure hashing library

# Introduction

**MHash-384** is a fast, portable and secure hashing library, released under the *MIT license*.

The MHash-384 core library has been written in plain **C**, and the CLI front-end has been written in **C++**. The core library provides a simple "stream processing" API, that is available in two flavors: a plain C99 version and an *object-oriented* C++ wrapper. Either way, the MHash-384 library produces hash values with a fixed length of 384 bits (48 bytes).

MHash-384 supports a wide range of compilers, including MSVC++, GCC (MinGW/Cygwin), Clang/LLVM and Intel C++. It also runs on many platforms, including Windows and Linux. Furthermore, the MHash-384 library has already been *ported* to various other programming languages, including **Microsoft.NET**, **Java**, **Delphi** and **Free Pascal** as well as **Python**.


# Quick Start Guide

In order to use the *MHash-384* library, simply include the header file `mhash384.h` in your code:

	#include <mhash384.h>

## Example for C language

If you source code is written in **C**, use the **`mhash384_t`** struct and the **`mhash384_XYZ()`** functions:

	/*variables*/
	uint8_t buffer[BUFFSIZE];
	size_t len;
	uint8_t result[MHASH384_SIZE];
	mhash384_t mhash384;
	
	/*initialization*/
	mhash384_init(&mhash384);
	
	/*input data processing*/
	while(more_data())
	{
		len = read_data(buffer, BUFFSIZE);
		mhash384_update(&mhash384, buffer, len);
	}
	
	/*finalization*/
	mhash384_final(&mhash384, result);

## Example for C++ language

Or, if you source code is written in **C++**, use the provided **`MHash384`** wrapper class:

	/*instance*/
	MHash384 mhash384;
	
	/*input data processing*/
	while(more_data())
	{
		const std::vector<uint8_t> buffer = read_data();
		mhash384.update(buffer);
	}
	
	/*finalization*/
	const uint8_t *result = mhash384.finish();


# Command-line Usage

**TODO**


# License

**MHash-384 - Simple fast portable secure hashing library**  
**Copyright(c) 2016-2019 LoRd_MuldeR <mulder2@gmx.de>**

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software
	and associated documentation files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use, copy, modify, merge, publish,
	distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or
	substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
	BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
	DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

<https://opensource.org/licenses/MIT>


# Version History

## Version 2.0.0 [????-??-??]

* Initial release of the 2.x development cycle

&nbsp;

[■](https://www.youtube.com/watch?v=dng06ZqI4Ss)
