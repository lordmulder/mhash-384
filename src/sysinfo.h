/* ---------------------------------------------------------------------------------------------- */
/* MHash-384 - System information                                                                 */
/* Copyright(c) 2016-2017 LoRd_MuldeR <mulder2@gmx.de>                                            */
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

#ifndef MHASH_CLI_SYSINFO_INCLUDED
#define MHASH_CLI_SYSINFO_INCLUDED

/*Includes*/
#include <stdint.h>

/*System*/
#if defined (__CYGWIN__)
#define SYSTEM_TYPE "Cygwin"
#elif defined(__MINGW32__)
#define SYSTEM_TYPE "MSYS/MinGW"
#elif defined(_WIN32) || defined(_WIN64)
#define SYSTEM_TYPE "Windows"
#elif defined(__linux__)
#define SYSTEM_TYPE "Linux"
#elif defined(__unix__)
#define SYSTEM_TYPE "Unix"
#else
#define SYSTEM_TYPE "Unknown"
#endif

/*Compiler*/
#ifdef __INTEL_COMPILER
#define COMPILER_TYPE "Intel(R) C/C++ Compiler"
#define COMPILER_VERS_MAJOR ((uint32_t)__INTEL_COMPILER)
#define COMPILER_VERS_MINOR (((uint32_t)__INTEL_COMPILER_BUILD_DATE) / ((uint32_t)10000))
#define COMPILER_VERS_PATCH (((uint32_t)__INTEL_COMPILER_BUILD_DATE) % ((uint32_t)10000))
#if defined(_M_X64) || defined(__x86_64__)
#define COMPILER_ARCH "x64"
#else
#define COMPILER_ARCH "x86"
#endif
#else
#ifdef _MSC_FULL_VER
#define COMPILER_TYPE "Microsoft(R) C/C++ Compiler"
#define COMPILER_VERS_MAJOR (((uint32_t)_MSC_FULL_VER) / ((uint32_t)10000000))
#define COMPILER_VERS_MINOR ((((uint32_t)_MSC_FULL_VER) / ((uint32_t)100000)) % ((uint32_t)100))
#define COMPILER_VERS_PATCH (((uint32_t)_MSC_FULL_VER) % ((uint32_t)100000))
#ifdef _M_X64
#define COMPILER_ARCH "x64"
#else
#define COMPILER_ARCH "x86"
#endif
#endif
#ifdef __GNUC__
#define COMPILER_TYPE "GNU Compiler Collection"
#define COMPILER_VERS_MAJOR ((uint32_t)__GNUC__)
#define COMPILER_VERS_MINOR ((uint32_t)__GNUC_MINOR__)
#define COMPILER_VERS_PATCH ((uint32_t)__GNUC_PATCHLEVEL__)
#ifdef __x86_64__
#define COMPILER_ARCH "x64"
#else
#define COMPILER_ARCH "x86"
#endif
#endif
#endif

#endif /*MHASH_CLI_SYSINFO_INCLUDED*/
