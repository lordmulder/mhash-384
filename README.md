---
title: "![MHash-384](img/mhash384/MHash-384.jpg)"
subtitle: "**Yet another simple fast portable secure hashing library**"
---


# Introduction

**MHash-384** is a fast, portable and secure hashing library, released under the *MIT license*.

The MHash-384 core library has been written in plain **C**, and the CLI front-end has been written in **C++**. The core library provides a simple "stream processing" API, that is available in two flavors: a plain C99 version and an *object-oriented* C++ wrapper. Either way, the MHash-384 library produces hash values with a fixed length of 384 bits (48 bytes).

MHash-384 supports a wide range of compilers, including MSVC++, GCC, MinGW/Cygwin, Clang/LLVM and Intel C++. It also runs on many platforms, including Windows, Linux, BSD and Solaris. Furthermore, the MHash-384 library has already been *ported* to various other programming languages, including **Java**, **Microsoft.NET**, **Python** as well as **Delphi**.


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

MHash-384 comes with a simple "standalone" command-line application, similar to the `sha1sum` or`sha256sum` utilities.

## Synopsis

The MHash-384 command-line application takes a number of optional options followed by an one or more input files.

If **no** input file is specified, input will be read from standard input (*stdin*).

The digest will be written to the standard output (*stdout*). Diagnostic message are written to the standard error (*stderr*).

	mhash384 [OPTIONS] [<FILE_1> <FILE_2> ... <FILE_N>]

## Options

MHash-384 supports the following options:

* **`--keep-going`**  
  Try to proceed with the remaining files, if a file has failed (e.g. access denied or file not found).  
  Only applicable, if *multiple* files have been specified. Default behavior is to abort immediately when a file has failed.

* **`--short`**  
  Print the digest in *short* format (**no** file names). Default format prints the digest followed by the file name.

* **`--lower-case`**  
  Print the digest in *lower-case* letters. Default format prints the digest in *upper-case* letters.  
  This option can **only** be used with the default *Hex* (hexadecimal) output format.

* **`--base64`**  
  Print the digest in [**Base64**](https://en.wikipedia.org/wiki/Base64) (RFC-4648) format. Default prints the digest in [**Hex**](https://en.wikipedia.org/wiki/Hexadecimal) (hexadecimal) output format.  
  This option **must not** be combined with the `--base85` option, for obvious reasons.

* **`--base85`**  
  Print the digest in [**Base85**](https://en.wikipedia.org/wiki/Ascii85) (Ascii85) format. Default prints the digest in [**Hex**](https://en.wikipedia.org/wiki/Hexadecimal) (hexadecimal) output format.  
  This option **must not** be combined with the `--base64` option, for obvious reasons.

* **`--help`**  
  Print the help screen (manpage) and exit program.

* **`--version`**  
  Print the program version (plus some build information) and exit program.

* **`--self-test`**  
  Run self-test and exit program. This will process various standard test vectors and validate the resulting hashes.  
  *Note:* Some test vectors contain very long inputs, therefore the computation can take a while to complete!

* **`--stress`**  
  Enable stress test mode. This will process all test strings from the specified input file, expecting one string *per line*.  
  All computed hash values are added to an `std::unordered_set`, thus checking for possible collisions.

* **`--benchmark`**  
  Measure the overall time required for the operation. If specified, output the total amount of time elapsed, in seconds.

## Output Format

In default operation mode, MHash-384 writes one line per input file to the standard output:

	<HASH_VALUE> [SPACE SPACE <FILE_NAME>]

The format of the hash value is either [**Hex**](https://en.wikipedia.org/wiki/Hexadecimal) (hexadecimal) or [**Base64**](https://en.wikipedia.org/wiki/Base64) (RFC-4648), depending on the specified options.

Also, the file name will be printed, unless "short" format was requested. File names *may* contain a path!

#### Sample output {-}

	BD41A203A61FE74178A8D507...33E553FD1569ED733C52BE8B  debian-7.9.0-amd64-DVD-1.iso
	EE328DDD4E116165252F1FF8...11729801097C51FB61D20184  debian-7.9.0-i386-DVD-1.iso
	A8B2007537867BDA0C18A264...45A1379AB8B4A77F9D8C8B24  debian-10.0.0-amd64-DVD-1.iso

## Exit Code

On success, *zero* is returned. On error or user interruption, a *non-zero* error code is returned.

Note that, with "keep going" mode enabled, the exit code reflects whether *at least* one file was processed successfully.

## Examples

Compute MHash-384 hash of a single file:

	mhash384 "C:\Images\debian-8.3.0-amd64-DVD-1.iso"

Compute MHash-384 hash of *two* files:

	mhash384 "C:\Images\debian-8.3.0-amd64-DVD-1.iso" "C:\Images\debian-8.3.0-i386-DVD-1.iso"

Compute MHash-384 hash of *multiple* files, using wildcard expansion ([globbing](https://en.wikipedia.org/wiki/Glob_(programming))):

	mhash384 "C:\Images\*.iso"

Compute MHash-384 hash from data passed directly via [pipeline](https://en.wikipedia.org/wiki/Pipeline_(Unix)):

	dd if=/dev/urandom bs=100 count=1 | mhash384


# API Specification

## Global definitions

Global definitions for both, the C and C++, API's.

### MHASH384_SIZE

The size of the final MHash-384 hash value (digest), in bytes. This value is qual to `48U`.

### MHASH384_WORDS

The number of words per MHash-384 hash. Each word has a size of 64 bits (`uint64_t`). This value is qual to `6U`.

## API for C language

All functions described in the following are *reentrant* and *thread-safe*. A single thread may compute multiple MHash-384 hashes in an "interleaved" fashion, provided that a separate MHash-384 context is used for each ongoing hash computation. Multiple threads may compute multiple MHash-384 hashes in parallel, provided that each thread uses its own separate MHash-384 context; *no* synchronization is required. However, sharing the same MHash-384 context between multiple threads is **not** safe in the general case. If the same MHash-384 context needs to be accessed from multiple threads, then the threads need to be synchronized explicitly (e.g. via Mutex lock), ensuring that all access to the shared context is rigorously serialized!

### mhash384_t

	typedef struct mhash384_t;

The MHash-384 context. It represents all state of an ongoing MHash-384 hash computation. Each MHash-384 hash computation needs a corresponding MHash-384 context. It is possible to re-use an MHash-384 context for multiple MHash-384 hash computations, provided that those hash computations are strictly serialized. If multiple MHash-384 hash computations need to be performed in an "interleaved" fashion, each ongoing hash computation needs to use its own separate MHash-384 context. In any case, the memory for the `mhash384_t` instance(s) must be allocated by the calling application. If the MHash-384 context was allocated on the heap space, the calling application also is responsible for freeing up that memory.

*Note:* Applications should treat this data-type as *opaque*, i.e. the application **must not** access the fields of the struct directly!

### mhash384_init()

	void mhash384_init(mhash384_t *const ctx);

Set up the MHash-384 hash computation. This function initializes the MHash-384 context; it prepares the state for the upcoming hash computation. The application is required to call this function *once* for each MHash-384 hash computation. The function must to be called ***before*** any input data can be processed in a specific MHash-384 context! The application may call this function again, on the same MHash-384 context, which will *reset* that context and start a new hash computation.

*Parameters:*

* `mhash384_t *ctx`  
  Pointer to the MHash-384 context of type `mhash384_t` that will be initialized (reset) by this operation.  
  *Note:* The MHash-384 library does **not** allocate the required memory; it must be allocated by the calling application!

### mhash384_update()

	void mhash384_update(mhash384_t *const ctx, const uint8_t *const data_in, const size_t len);

Process next chunk of input data. This function performs the actual MHash-384 hash computation, in an incremental way. The function processes the next **N** bytes of input data and updates the MHash-384 context (`mhash384_t`) accordingly. The application is supposed to call this function in a loop, with the *same* MHash-384 context, until all input has been processed.

*Parameters:*

* `mhash384_t *ctx`  
  Pointer to the hash computation state of type `mhash384_t` that will be updated by this operation.

* `const uint8_t *data_in`  
  Pointer to the input data to be processed by this operation. The input data needs to be located in one continuous block of memory. The given pointer specifies the *base address*, i.e. the address of the *first* byte to be processed.  
  *Note:* Formally, the input data is defined as an array of byte (`uint8_t`). Nonetheless, *any* kind of input data can be processed, by applying the proper *typecast* operator. For numeric values, the platform's [endianness](https://en.wikipedia.org/wiki/Endianness) applies!

* `size_t len`  
  The *length* of the input data to be processed, *in bytes*. Specify `sizeof(T) * count` for data types **T** other than byte.  
  *Note:* All *bytes* in the range from `data_in[0]` up to and including `data_in[len-1]` will be processed as input.

### mhash384_final()

	void mhash384_final(mhash384_t *const ctx, uint8_t *const digest_out);

Retrieve final hash value. This function completes the MHash-384 hash computation and returns the computed hash value. The function finalizes the MHash-384 context (`mhash384_t`) and writes the resulting hash value to the output buffer. Once this function has been called, the corresponding MHash-384 context will be in an ***undefined*** state, until it is [reset](#mhash384_init)!

*Parameters:*

* `mhash384_t *ctx`  
  Pointer to the hash computation state of type `mhash384_t` that will be finalized by this operation.
  *Note:* The MHash-384 library does **not** free this memory; it may need to be freed up by the calling application!

* `uint8_t *digest_out`  
  Pointer to the memory block where the final MHash-384 hash (digest) is to be stored. This memory needs to be allocated by the calling application! The size of the MHash-384 hash value, in bytes, is equal to `MHASH384_SIZE`.  
  *Note:* All *bytes* ranging from `digest_out[0]` up to and including `digest_out[MHASH384_SIZE-1]` will be overwritten!

### mhash384_compute()

	void mhash384_compute(uint8_t *const digest_out, const uint8_t *const data_in, const size_t len);

Compute hash value at once. This is a convenience function that can be used to compute an MHash-384 hash value with just a single invocation. The function processes a block of **N** input bytes and writes the resulting hash value to the output buffer. This function does *not* required the caller to provide an MHash-384 context; it internally uses a "transient" context. Anyway, this function is fully thread-safe. Naturally, this function is *only* applicable where *all* input data is available at once.

*Parameters:*

* `uint8_t *digest_out`  
  Pointer to the memory block where the final MHash-384 hash (digest) is to be stored. This memory needs to be allocated by the calling application! This size of the MHash-384 hash value, in bytes, is equal to `MHASH384_SIZE`.  
  *Note:* All *bytes* ranging from `digest_out[0]` up to and including `digest_out[MHASH384_SIZE-1]` will be overwritten!

* `const uint8_t *data_in`  
  Pointer to the input data to be processed by this operation. The input data needs to be located in one continuous block of memory. The given pointer specifies the *base address*, i.e. the address of the *first* byte to be processed.  
  *Note:* Formally, the input data is defined as an array of byte (`uint8_t`). Nonetheless, *any* kind of input data can be processed, by applying the proper *typecast* operator. For numeric values, the platform's [endianness](https://en.wikipedia.org/wiki/Endianness) applies!

* `size_t len`  
  The *length* of the input data to be processed, *in bytes*. Specify `sizeof(T) * count` for data types **T** other than byte.
  *Note:* All *bytes* in the range from `data_in[0]` up to and including `data_in[len-1]` will be processed as input.

### mhash384_version()

	void mhash384_version (uint16_t *const major, uint16_t *const minor, uint16_t *const patch);

Retrieve version information. This function returns the current version of the MHash-384 library.

*Parameters:*

* `uint16_t *major`  
  Pointer to a variable of type `uint16_t` where the *major* version of the MHash-384 library will be stored.

* `uint16_t *minor`  
  Pointer to a variable of type `uint16_t` where the *minor* version of the MHash-384 library will be stored.

* `uint16_t *patch`  
  Pointer to a variable of type `uint16_t` where the *patch* level of the MHash-384 library will be stored.

### mhash384_selftest()

	bool mhash384_selftest(void);

Self-test routine. This function runs the built-in self-test of the MHash-384 library; intended for debugging purposes.

*Return value:*

* Returns `true`, if the self-test completed successfully; returns `false`, if any problems have been detected.

## API for C++ language

For the C++ langauge, the **`MHash384`** class is provided, as a convenience wrapper around the C-API. All functions of the `MHash384` class are *reentrant* and *thread-safe*. A single thread may compute multiple MHash-384 hashes in an "interleaved" fashion, provided that a separate `MHash384` instance (object) is used for each ongoing hash computation. Multiple threads may compute multiple MHash-384 hashes in parallel, provided that each thread uses its own separate `MHash384` instance; *no* synchronization is required. However, sharing the same `MHash384` instance between multiple threads is **not** safe in the general case. If the same `MHash384` instance needs to be accessed from multiple threads, then the threads need to be synchronized explicitly (e.g. via Mutex lock), ensuring that all access to the shared instance is rigorously serialized!

### MHash384()

	MHash384(void)

Constructor. Creates a new `MHash384` instance (object) and prepares the state for the upcoming hash computation. Each instance *internally* maintains the corresponding MHash-384 context. The application is required to create a separate `MHash384` instance for each ongoing MHash-384 hash computation; it is possible to re-use an `MHash384` instance for multiple MHash-384 hash computations, provided that those hash computations are strictly serialized.  
*Note:* The application is required to allocate the memory for the `MHash384` instance. If the instance was allocated on the heap (*dynamic* storage duration), the application is also required to explicitly destroy the instance, when no longer needed.

### MHash384::update() [1]

	void MHash384::update(const std::uint8_t *const data, const size_t len)

Process next chunk of input data. This function performs the actual MHash-384 hash computation, in an incremental way. The function processes the next **N** bytes of input data and updates the internal MHash-384 context accordingly. The application is supposed to call this function in a loop, on the *same* `MHash384` instance, until all input has been processed.

*Parameters:*

* `const uint8_t *data_in`  
  Pointer to the input data to be processed by this operation. The input data needs to be located in one continuous block of memory. The given pointer specifies the *base address*, i.e. the address of the *first* byte to be processed.  
  *Note:* Formally, the input data is defined as an array of byte (`uint8_t`). Nonetheless, *any* kind of input data can be processed, by applying the proper *typecast* operator. For numeric values, the platform's [endianness](https://en.wikipedia.org/wiki/Endianness) applies!

* `size_t len`  
  The *length* of the input data to be processed, *in bytes*. Specify `sizeof(T) * count` for data types **T** other than byte.  
  *Note:* All *bytes* in the range from `data_in[0]` up to and including `data_in[len-1]` will be processed as input.

### MHash384::update() [2]

	template<size_t size>
	void MHash384::update(const std::array<std::uint8_t, size> &data)

A convenience overload of the [`MHash384::update()`](#mhash384update-1) function, which processes an `std::array<uint8_t, N>` as input.

*Parameters:*

* `const std::array<uint8_t, N> &data`  
  Read-only reference to the `std::array<uint8_t, N>` containing the input data to be processed.  
  *Note:* All bytes in the range from `data[0]` up to and including `data[data.size()-1]` will be processed as input.

### MHash384::update() [3]

	void MHash384::update(const std::vector<std::uint8_t> &data)

A convenience overload of the [`MHash384::update()`](#mhash384update-1) function, which processes an `std::vector<uint8_t>` as input.

*Parameters:*

* `const std::vector<std::uint8_t> &data`  
  Read-only reference to the `std::vector<uint8_t>` containing the input data to be processed.  
  *Note:* All bytes in the range from `data[0]` up to and including `data[data.size()-1]` will be processed as input.

### MHash384::update() [4]

	void MHash384::update(const std::string &text)

A convenience overload of the [`MHash384::update()`](#mhash384update-1) function, which processes an `std::string` as input.

*Parameters:*

* `const std::string &text`  
  Read-only reference to the `std::string` containing the input data to be processed.  
  *Note:* All characters in the range from `text[0]` up to and including `text[text.length()-1]` will be processed as input. Each character in the `std::string` is processed as a *byte* value, disregarding any specific character encoding.

### MHash384::update() [5]

	void MHash384::update(const char *const text)

A convenience overload of the [`MHash384::update()`](#mhash384update-1) function, which processes a NULL-terminated C string as input.

*Parameters:*

* `const char *const text`  
  Read-only pointer to the first character of the NULL-terminated string to be processed.  
  *Note:* All characters in the range from `text[0]` up to and including `text[strlen(text)-1]` will be processed as input. Each character in the C string is processed as a *byte* value, disregarding any specific character encoding.

### MHash384::update() [6]

	template<typename element_type>
	void MHash384::update(const element_type *const address);

A convenience overload of the [`MHash384::update()`](#mhash384update-1) function, which processes an object designated by a pointer.

*Parameters:*

* `const element_type *const address`  
  Read-only pointer to the target object to be processed.  
  *Note:* The given object is processed as a byte-sequence, like a POD; all bytes in the range from `address[0]` up to and including `address[sizeof(element_type)-1]` will be processed as input.

### MHash384::update() [7]

	template<typename element_type>
	void MHash384::update(const element_type &element);

A convenience overload of the [`MHash384::update()`](#mhash384update-1) function, which processes an object designated by a reference.

*Parameters:*

* `const element_type &element`  
  Read-only reference to the target object to be processed.  
  *Note:* The given object is processed as a byte-sequence, like a POD; all bytes in the range from `addr[0]` up to and including `addr[sizeof(element_type)-1]` with `addr = std::addressof(element)` will be processed as input.

### MHash384::update() [8]

	template<typename iterator_type>
	void MHash384::update(const iterator_type &first, const iterator_type &last)

A convenience overload of the [`MHash384::update()`](#mhash384update-1) function, which processes a sequence of elements via iterators.

*Parameters:*

* `const iterator_type &first`  
  Read-only reference to the iterator designating the *first* element to be processed.  
  *Note:* All elements in the range from `*first` up to but excluding `*last` will be processed as input. Each element in this range is processed as a byte-sequence, like a POD, assuming a size of `sizeof(iterator_type::value_type)`.

* `const iterator_type &last`  
  Read-only reference to the iterator designating the element just after the *last* element to be processed.  
  *Note:* All elements in the range from `*first` up to but excluding `*last` will be processed as input. Each element in this range is processed as a byte-sequence, like a POD, assuming a size of `sizeof(iterator_type::value_type)`.

### MHash384::finish()

	const std::uint8_t *MHash384::finish(void)

Retrieve final hash value. This function completes the MHash-384 hash computation. The function finalizes the internal MHash-384 context, if it was not finalized yet, and returns a pointer to the buffer containing the resulting hash value. Once this function has been called, the `MHash384` instance remains in the *finalized* state, until it is [reset](#mhash384reset).

***Warning:*** Trying to process more input data while the `MHash384` instance is in *finalized* state will throw an exception!

*Return value:*

* Returns a read-only pointer to the internal buffer containing the final hash value; this buffer is owned by the `MHash384` instance. The size of the MHash-384 hash value, in bytes, is equal to `MHASH384_SIZE`.  
  *Note:* This pointer remains valid only until the `MHash384` instance is [reset](#mhash384reset) or destroyed. If the hash value needs to be retained after the instance was reset/destroyed, the application must copy the hash value to a separate buffer!

### MHash384::reset()

	void MHash384::reset(void)

Reset the MHash-384 hash computation. This function re-initializes the internal MHash-384 context, thus starting a new MHash-384 hash computation. It is **not** necessary to explicitly call this function on a new `MHash384` instance; it is called implicitly by the constructor. However, it is possible to re-use an existing `MHash384` instance for multiple (strictly serialized) MHash-384 hash computations, by calling this function in between each pair of consecutive hash computations.


# Supported platforms

MHash-384 has been tested to successfully build and run on (at least) the following platforms:

## C/C++ library and CLI front-end

* **Microsoft Windows (x84/x64)**
    - Microsoft Visual C++, version 16.00 (Visual Studio 2010) or newer
    - Mingw-w64 (from MSYS2), tested with version 8.0.0, using GCC 9.2.0 or Clang 9.0.0
    - MinGW (mingw.org), tested with version 5.3.2, using GCC 8.2.0
    - Cygwin, tested with version 3.1.2 (x64), using GCC 7.4.0 or Clang 8.0.1

* **Linux/GNU (x86/x64)**
    - Ubuntu, tested with version 16.04 (Xenial), using GCC 5.4.0 or Clang 3.8.0
    - CentOS/Red Hat Enterprise Linux, tested with version 8.1, using GCC 8.3.1 or Clang 8.0.1
    - Manjaro, tested with version 18.1.5, using GCC 9.2.0 or Clang 9.0.1
    - openSUSE (Leap), tested with version 15.1, using GCC 7.5.0 or Clang 7.0.1

* **BSD-Family (x86/x64)**
    - FreeBSD, tested with TrueOS version 18.12, using GCC 7.4.0 or Clang 7.0.1
    - OpenBSD, tested with version 6.6, using GCC 8.2.0 or Clang 8.0.1

* **Solaris (x86/x64)**
    - OpenSolaris/Illumos, tested with OpenIndiana version 2019.10, using GCC 9.2.0

## Ports to other lanuguages

* **Java**
    - Java SE 8, tested with OpenJDK Runtime Environment 1.8.0_242
    - Java SE 11, tested with OpenJDK Runtime Environment 11.0.5

* **.NET Framework**
    - Microsoft.NET Framework 4.5 (or newer), tested with Visual Studio 2019 (version 15.9.20)
    - Mono, tested with Mono C# Compiler version 5.18.0 (Ubuntu 19.10)

* **Python**
    - CPython 3.x, tested with version 3.8.1
    - PyPy (Python 3.6 compatible), tested with version 7.3.0, highly recommended for improved performance!

* **Delphi (Object Pascal)**
    - Bordland Delphi, tested with Delphi 7.1 (DCC 15.00)
    - Lazarus/Free Pascal, tested with Lazarus 2.06 (Free Pascal Compiler 3.04)


# Build instructions

The MHash-384 C/C++ library and CLI front-end can be built using (at least) one of the following build systems:

## Microsoft Visual C++

MHash-384 can be built for the Windows platform using the **Microsoft Visual C++** compiler, version 16.00 or later.

The provided project/solution files should build successfully with *Visual Studio 2010* or later. However, be aware that it may be necessary to adjust the "Platform Toolset" to your specific version of Visual Studio in all projects! Build configurations are available for both, *32-Bit* (`Win32`) and *64-Bit* (`x64`) Windows, but the 64-Bit flavor is recommended for best performance.

Note: You can download the latest version of the Visual Studio "Community" edition for free from the [official web-site](https://visualstudio.microsoft.com/).

### Command-line usage

Building MHash-384 from the developer command prompt is supported via the MSBuild tool:

    MSBuild.exe /property:Configuration=Release /property:Platform=x64 /target:Rebuild "MHash384.sln"

## GNU  C/C++ compiler

MHash-384 can be built using the **GNU  C/C++ compiler (GCC)**, version 4.8 or later, or any GCC-compatible compiler, such as Clang/LLVM, on a wide range of platforms; supported platforms include Linux, the BSD family, Solaris and Windows.

The provided makefiles should build successfully with GNUmake on any supported platform. GNUmake is the default `make` implementation on Linux/GNU, but may need to be installed separately and invoked as `gmake` on BSD and Solaris. GCC or a GCC-compatible compiler (e.g. Clang/LLVM) is available out-of-the-box on most supported platforms; otherwise it can usually be installed from the system's package manager. Please see the documentation of your specific distribution for details!

### Command-line usage

In order to build MHash-384, simply run **`make`** from the MHash-384 base directory, for example:

    $ make -B MARCH=x86-64 MTUNE=intel STATIC=1

### Make file parameters

The following options can be used to tweak the behavior of the provided makefiles:

* **`MARCH`**: Generate machine code for the specified CPU type, see [*-march*](https://gcc.gnu.org/onlinedocs/gcc-9.2.0/gcc/x86-Options.html#index-march-14) for details (default is `native`)
* **`MTUNE`**: Tune the generated machine code for the specified CPU type, see [*-mtune*](https://gcc.gnu.org/onlinedocs/gcc-9.2.0/gcc/x86-Options.html#index-mtune-16) for details (default is `native`)
* **`STATIC`**: If set to `1`, link with *static* CRT libraries; otherwise link with *shared* CRT libraries (default is `0`)
* **`DEBUG`**: If set to `1`, generate a binary suitable for debugging; otherwise generate an optimized binary (default is `0`)
* **`NODOCS`**: If set to `1`, the HTML documents are **no** generated; useful where pandoc is unavailable (default is `0`)
* **`CXX`**: The C++ compiler to be used (default is *system-specific*, e.g. `g++` or `clang++`)
* **`AR`**: The archiver to be used (default is *system-specific*, usually `ar`)
* **`STRIP`**: The strip program to be used (default is `strip`)
* **`WNDRS`**: The Windows resource compiler to be used, used on Cygwin and MinGW only (default is `windres`)
* **`PNDOC`**: The pandoc document converter to be used (default is `pandoc`)
* **`TAR`**: The tarball program to be used (default is `tar`)
* **`SANITIZE`**: Instrument the binary with the specified sanitizer, e.g. `address` to enable [*AddressSanitizer*](https://gcc.gnu.org/onlinedocs/gcc-9.2.0/gcc/Instrumentation-Options.html#index-fsanitize_003daddress) (*no* default)

### Windows support

It is possible to build MHash-384 with GCC or Clang/LLVM on the Windows platform thanks to [Cygwin](https://www.cygwin.com/) or [MinGW/MSYS](http://www.mingw.org/wiki/msys). However, if you want to build with GCC or Clang/LLVM on Windows nowadays, then it is *highly recommended* to use [**MSYS2**](https://www.msys2.org/) in conjunction with [**Mingw-w64**](http://mingw-w64.org/) – even for 32-Bit targets! The “old” Mingw.org (Mingw32) project is considered *deprecated*.

Just follow the basic **MSYS2** setup procedure, as described on the [official web-site](https://github.com/msys2/msys2/wiki/MSYS2-installation), then install Mingw-w64:

    pacman -S base-devel mingw-w64-i686-toolchain mingw-w64-x86_64-toolchain


# License

**MHash-384 - Simple fast portable secure hashing library**  
**&copy; 2016-2020 LoRd_MuldeR [&lt;mulder2@gmx.de&gt;](mailto:mulder2@gmx.de)**

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
