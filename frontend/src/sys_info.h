/* ---------------------------------------------------------------------------------------------- */
/* MHash-384 - Simple fast portable secure hashing library                                        */
/* Copyright(c) 2016-2020 LoRd_MuldeR <mulder2@gmx.de>                                            */
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

#ifndef INC_MHASH384_SYSINFO_H
#define INC_MHASH384_SYSINFO_H

/*
 * Detect operating system
 */
#if defined(_WIN32)
#	define SYSTEM_NAME "Win"
#elif defined(__CYGWIN__)
#	define SYSTEM_NAME "Cygwin"
#elif defined(__linux__)
#	define SYSTEM_NAME "Linux"
#else
#	error Unknonw platform!
#endif

/*
 * Detect CPU architecture
 */
#if defined(_M_X64) || defined(__x86_64__) || defined(__x86_64) || defined(__amd64)
#	define SYSTEM_ARCH "x64"
#elif defined(_M_IX86) || defined(__i386__) || defined(__i386) || defined(__X86__) || defined(_X86_)
#	define SYSTEM_ARCH "x86"
#elif defined(_M_ARM) || defined(__arm__) || defined(__arm) || defined(_ARM)
#	define SYSTEM_ARCH "ARM"
#elif defined(_M_ARM64) || defined(__aarch64__)
#	define SYSTEM_ARCH "ARM64"
#elif defined(_M_PPC) || defined(__powerpc__) || defined(__ppc__) || defined(_ARCH_PPC)
#	define SYSTEM_ARCH "PPC"
#elif defined(__powerpc64__) || defined(__ppc64__) || defined(_ARCH_PPC64)
#	define SYSTEM_ARCH "PPC64"
#else
#	error Unknonw architecture!
#endif

/*
 * Detect compiler and version
 */
#if defined(__clang__)
#	if defined(__llvm__)
#		define COMPILER_FMT "Clang/LLVM %u.%u.%u"
#	elif defined(__c2__)
#		define COMPILER_FMT "Clang/C2 %u.%u.%u"
#	else
#		error Unknown Clang-based compiler detected!
#	endif
#	define COMPILER_ARG __clang_major__, __clang_minor__, __clang_patchlevel__
#elif defined(__INTEL_COMPILER)
#	if defined(__INTEL_COMPILER_UPDATE) && (__INTEL_COMPILER_UPDATE > 0)
#		define COMPILER_FMT "Intel C++ Compiler %u.%02u.%u"
#		define COMPILER_ARG __INTEL_COMPILER / 100U, (__INTEL_COMPILER % 100U), __INTEL_COMPILER_UPDATE
#	else
#		define COMPILER_FMT "Intel C++ Compiler %u.%02u"
#		define COMPILER_ARG __INTEL_COMPILER / 100U, (__INTEL_COMPILER % 100U)
#	endif
#elif defined(__GNUC__)
#	if defined(__MINGW32__)
#		define COMPILER_FMT "MinGW/GCC %u.%u.%u"
#	elif defined(__CYGWIN__)
#		define COMPILER_FMT "Cygwin/GCC %u.%u.%u"
#	else
#		define COMPILER_FMT "GNU C++ Compiler %u.%u.%u"
#	endif
#	define COMPILER_ARG __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__
#elif defined(_MSC_VER)
#	if defined(_MSC_BUILD) && (_MSC_BUILD > 0)
#		define COMPILER_FMT "Microsoft C++ Compiler %u.%02u.%u.%u"
#		define COMPILER_ARG _MSC_VER / 100U, _MSC_VER % 100U, _MSC_FULL_VER % 1000000U, _MSC_BUILD
#	else
#		define COMPILER_FMT "Microsoft C++ Compiler %u.%02u.%u"
#		define COMPILER_ARG _MSC_VER / 100U, _MSC_VER % 100U, _MSC_FULL_VER % 1000000U
#	endif
#else
#	error Unknonw compiler type!
#endif

#endif /*INC_MHASH384_SYSINFO_H*/
