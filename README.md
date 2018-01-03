% ![](img/mhash/MHash-384.jpg)  
MHash-384
% Simple fast portable header-only hashing library

# Introduction

**MHash-384** is a fast, portable and secure *header-only* hash library, released under the *MIT license*. It provides a very simple "stream processing" API and produces hash values with a length of 384 bits (48 bytes).

The MHash-384 library has originally been written for **C** and **C++**. It provides a "plain C" API as well as an *object-oriented* C++ wrapper. It also supports many compilers (MSVC, GCC, MinGW, etc.) on various platforms (Windows, Linux, etc).

Furthermore, the MHash-384 library has already been *ported* to various other programming languages. This currently includes the **Microsoft.NET** platform (C#, VB.NET, etc.), **Java**, **Delphi** (Pascal) as well as **Python**.


# Quick Start Guide

In order to use the *MHash-384* library, simply include the header file `mhash_384.h` in your *C* or *C++* source code file.

This is the *only* file you are going to need. Being a [*header-only*](https://en.wikipedia.org/wiki/Header-only) library, MHash-384 does **not** require any additional library files to be linked to your program. Also, **no** additional DLL files (or *shared objects*) are required at runtime.

	#include <mhash_384.h>

## Example for C language

If you source code is written in plain **C**, simply use the provided *global* functions:

	/*variables*/
	const uint8_t *data_ptr;
	uint32_t data_len;
	uint8_t result[MHASH_384_LEN];
	mhash_384_t context;
	
	/*initialization*/
	mhash_384_initialize(&context);
	
	/*input data processing*/
	while(have_more_data())
	{
		data_ptr = fetch_next_data_chunk(&data_len);
		mhash_384_update(&context, data_ptr, data_len);
	}
	
	/*finalization*/
	mhash_384_finalize(&context, result);

## Example for C++ language

And, if you source code is written in **C++**, the *MHash384* class from *mhash* namespace is used:

	/*variables*/
	std::vector<uint8_t> data;
	uint8_t result[MHASH_384_LEN];
	
	/*construction*/
	mhash_384::MHash384 instance;
	
	/*input data processing*/
	while(source.have_more_data())
	{
		data = source.fetch_next_data_chunk();
		instance.update(data);
	}
	
	/*finalization*/
	instance.finalize(result);


# Command-line Usage

MHash-384 comes with a simple "standalone" command-line application. This program primarily serves as an example on how to use the library. However, the command-line application may also come in handy to quickly compute checksums (hashes) of local files. Furthermore, the MHash-384 program integrates nicely into the "pipes and filters" design pattern, by consuming arbitrary inputs from the standard input stream and writing hash values (as Hex string) to the standard output stream.

## Synopsis

The MHash-384 command-line application takes a number of optional options followed by an optional input file. If **no** input file is specified, or if input file is "-", input will be read from standard input stream (*stdin*).

	mhash_384 [options] [input_file]

## Options

MHash-384 supports the following options:

* **`-p`**, **`--progress`**  
  Print the total size of the input file and the percentage processed so far to *stderr* while hash computation is running.  
  If the total input size can **not** be determined (e.g. using pipe), the number of bytes processed so far is printed.

* **`-u`**, **`--upper`**  
  Output the final digest (hash) in *upper case* Hex letters. Default mode is *lower case*.

* **`-b`**, **`--bench`**  
  Compute performance statistics (e.g. bytes processed per second) and print them to the *stderr* at the end of the process.

* **`-v`**, **`--version`**  
  Print library version to the *stdout* and exit program.

* **`-t`**, **`--test`**  
  Run *built-in self-test* and exit program. Computes hashes from test vectors and compares results to reference hashes.

* **`-h`**, **`--help`**  
  Print help screen (man page) and exit program.

## Examples

Compute MHash-384 hash of a local file:

	mhash_384 C:\Images\debian-8.3.0-amd64-DVD-1.iso"

Compute MHash-384 hash of a local file, with more verbose status outputs:

	mhash_384 -p -b C:\Images\debian-8.3.0-amd64-DVD-1.iso"

Compute MHash-384 from random bytes, passing data directly from [`dd`](https://en.wikipedia.org/wiki/Dd_%28Unix%29) via pipe:

	dd if=/dev/urandom bs=100 count=1 | mhash_384


# Algoritm Description

This chapter describes the of the MHash-384 algorithm, which is a novel design (from the scratch), in full detail.

## Informal Description

We start with an informal description of the MHash-384 algorithm, which also explains the design goals:

### The XOR table

MHash-384 uses a table of *257* pre-computed 384-Bit words. This table is referred to as the *XOR-table*. It has been generated in such a way that each possible pair of 384-Bit words has a binary [Hamming distance](https://en.wikipedia.org/wiki/Hamming_distance) of *at least* 182 bits.

The MHash-384 digest of a given input message is computed in a *stream-like* fashion. This means that we start with the "empty" (zero) hash value, we will process the given message *byte by byte*, and we will "update" the hash value for each input byte. The final hash value of a message is the hash value that results after all of its bytes have been processed.

The "update" rule is defined as follows: We select the 384-Bit word from the XOR-table whose index matches the current input byte value, and we *combine* the selected 384-Bit word with the previous hash value in order to form the new (updated) hash value. If, for example, the current input byte equals `0x00`, then we select the *first* 384-Bit word from the XOR-table. If the current input byte equals `0x01`, then we select the *second* 384-Bit word from the XOR-table. And so on&hellip;

In any case, the selected 384-Bit word (from the XOR-table) will be combined with the previous hash value using the binary [XOR](https://en.wikipedia.org/wiki/Exclusive_or) (exclusive OR) function. XOR'ing the previous hash value with the selected 384-Bit word will effectively *flip* a certain sub-set of its bits &ndash; depending on the current input byte value. Because the 384-Bit words in the XOR-table have a guaranteed minimum Hamming distance to each other, each possible input byte value is also guaranteed to flip a *different* subset of bits.

### The MIX table

In fact the "update" rule is slightly more complex. That's because, in each update step, the previous hash value bytes additionally will be *shuffled* (permuted). The shuffling of the hash bytes will be performed immediately *before* XOR'ing the previous hash value with the select 384-Bit word from the XOR-table. Be aware that the shuffling ensures that the *same* input bytes are going results in a *different* hash value &ndash; with very high probability &ndash; when they are processed in a different order.

The shuffling procedure is implemented using the [Fisher-Yates](https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle) "in-place" shuffle algorithm. For this purpose, MHash-384 uses a table of *256* pre-computed permutations. This table is referred to as the *MIX-table*. Each of its rows (permutations) contains 48 Fisher-Yates shuffling indices, as required to shuffle the 48 hash bytes. The MIX-table has been generated in such a way, that each of the *256* permutations "moves" the elements (hash bytes) to different positions than all other permutations.

We use a counter that keeps track of the MIX-table row (permutation). The counter's value equals the zero-based index of the MIX-table row which is to be applied in the *next* update step. It is initialized to *zero* at the beginning, and it will be increased by one after each update step. After the *last* MIX-table row (i.e. index *996*) the counter will wrap around to index *zero* again.

### The RND table

*TODO*

### The SBX table

*TODO*

### Finalization

Last but not least, the computation of the MHash-384 digest is *finalized* by XOR'ing the current hash value with the very last 384-Bit word of the XOR-table. This 384-Bit word has index 256 and therefore can *never* be selected by any input byte value.

## Pseudocode

The MHash-384 algorithm can be summed up with the following simple pseudocode:

	procedure mhash384
	const:
	  HASH_SIZE = 48
	  TABLE_XOR: matrix[257 × HASH_SIZE] of byte
	  TABLE_MIX: matrix[256 × HASH_SIZE] of byte
	  TABLE_RND: matrix[256 × HASH_SIZE] of byte
	  TABLE_SBX: matrix[256 × HASH_SIZE] of byte
	input:
	  message: array[N] of byte
	output:
	  hash: array[HASH_SIZE] of byte
	vars:
	  ctr: integer
	  val: byte
	  tmp_src: array[HASH_SIZE] of byte
	  tmp_dst: array[HASH_SIZE] of byte
	begin
	  /*initialization*/
	  ctr ← 0
	  for i = 0 to HASH_SIZE - 1 do
	    (tmp_src[i], tmp_dst[i]) ← (0x00, 0x00)
	  done
	  
	  /*input message processing*/
	  for k = 0 to N - 1 do
	    for i = 0 to HASH_SIZE - 1 do
	      val ← tmp_src[TABLE_MIX[ctr,i]] ⊕ TABLE_XOR[message[k],i] ⊕ TABLE_RND[ctr,i]
	      tmp_dst[i] ← tmp_dst[i] ⊕ TABLE_SBX[val,i]
	    done
	    ctr ← (ctr + 1) mod 256
	    xchg tmp_src ⇄ tmp_dst
	  done
	  
	  /*finalization*/
	  for i = 0 to HASH_SIZE - 1 do
	    val ← tmp_src[i] ⊕ TABLE_XOR[256,i]
	    hash[i] ← TABLE_SBX[val,i]
	  done
	end.


# Detailed API Specification

Global definitions for both, C and C++, API's.

## Global definitions

### MHASH_384_LEN

This constant specifies the length of a MHash-384 digest (hash value) in bytes/octets. It is equal to `48UL`.

A memory block (array) that is intended to hold a MHash-384 digest, e.g. the final result of the hash computation, needs to be at least `MHASH_384_LEN` bytes in size.

### MHASH_384_VERSION_MAJOR

The MHash-384 library *major* version. Major release may change the API, so backwards compatibility is **not** guaranteed between different *major* versions.

Applications generally are written for a specific *major* version of the library.

### MHASH_384_VERSION_MINOR

The MHash-384 library *minor* version. Minor releases may add new features, but they do **not** change the API in a way that would break backwards compatibility.

Applications may require a certain minimum *minor* version of the library, but will work with higher *minor* versions too.

### MHASH_384_VERSION_PATCH

The MHash-384 library *patch* level. Patch releases may include bugfixes and optimizations, but they do **not** add new features or change the API.

Application code does **not** need to care about the *patch* level of the library.

## API for for C language

All functions described in the following are *reentrant*, but **not** *thread-safe*. This means that *multiple* hash computation *can* be performed safely in an "interleaved" fashion, as long as each hash computation uses its own separate state variable. Also, multiple hash computation *can* be performed safely in "concurrent" threads, as long as each thread uses its own separate state variable. If, however, the *same* state variable needs to be accessed from *different* "concurrent" threads, then the application **must** *serialize* the function calls, e.g. by means of a *mutex lock*.

### mhash_384_t

	typedef struct mhash_384_t;

The `mhash_384_t` data-type is used to maintain the hash computation state. Use one instance (variable) of `mhash_384_t` for each ongoing hash computation. The memory for the `mhash_384_t` instance must be allocated/maintained by the calling application.

*Note:* Applications should treat this data-type as *opaque*, i.e. the application **must not** access the fields of the struct directly, because `mhash_384_t` may be subject to change in future library versions!

### mhash_384_initialize()

	void mhash_384_initialize(mhash_384_t *const ctx);

This function is used to initialize (or to reset) the hash computation, i.e. it will set up the initial hash computation state. The application is required to call this function *once* for each hash computation. The function has to be called **before** any input data is processed!

*Parameters:*

* `mhash_384_t *ctx`  
  Pointer to the hash computation state of type `mhash_384_t` that will be initialized (reset) by this operation. The previous state will be lost!

### mhash_384_update()

	void mhash_384_update(mhash_384_t *const ctx, const uint8_t *const input, const size_t len);

This function is used to process the next **N** bytes (octets) of input data. It will update the hash computation state accordingly. The application needs to call this function repeatedly, on the *same* state variable (`mhash_384_t`), until all input data has been processed.

*Parameters:*

* `mhash_384_t *ctx`  
  Pointer to the hash computation state of type `mhash_384_t` that will be updated by this operation.

* `const uint8_t *input`  
  Pointer to the input data to be processed by this operation. The input data needs to be located in one continuous block of memory. The given pointer specifies the *base address* of the input data, i.e. the address of the *first* byte (octet) to be processed.  
  *Note:* Formally, the input data is defined as a sequence of `uint8_t`, i.e. a sequence of bytes (octets). However, *any* suitable *byte*-based input data can be processed using the proper *typecast* operator.

* `size_t len`  
  The *length* of the input data to be processed, in bytes (octets). All memory addresses in the range from `input` up to and including `input+(len-1)` will be processed as input. Applications need to carefully check `len` to avoid buffer overruns!

### mhash_384_finalize()

This function is used to finalize the hash computation and output the final digest (hash value). Typically, the application will call this function *once*, **after** all input data has been processed.

*Note:* The hash computation state is treated *read-only* by this function. This means that the application *may* call the function at any time to get an "intermediate" hash of all input bytes (octets) process *so far* and then continue to process more input bytes (octets).

	void mhash_384_finalize(const mhash_384_t *const ctx, uint8_t *const output);

* `const mhash_384_t *ctx`  
  Pointer to the hash computation state of type `mhash_384_t` from which the final digest is computed.

* `uint8_t *output`
  Pointer to the memory block where the final digest (hash value) will be stored. This memory needs to be allocated by the calling application! The digest will be written to the memory addresses from `output` up to and including `output+(MHASH_384_LEN-1)`.

## API for for C++ language

All classes described in the following are *reentrant*, but **not** *thread-safe*. This means that *multiple* hash computation *can* be performed safely in an "interleaved" fashion, as long as each hash computation uses its own separate object (instance). Also, multiple hash computation *can* be performed safely in "concurrent" threads, as long as each thread uses its own separate object (instance). If, however, the *same* object (instance) needs to be accessed from *different* "concurrent" threads, then the application **must** *serialize* the method calls, e.g. by means of a *mutex lock*.

*Note:* The classes described in the following live in the `mhash` namespace. Any functions, data-types or constants in the `mhash::internals` namespace should be regarded *opaque* by the application, as those may be subject to change in future library versions!

### Constructor

	MHash384::MHash384(void);

Constructs a new `MHash384` object sets up the initial hash computation state. The application is required to use the *same* `MHash384` object for the entire hash computation.

### update() [1]

	void MHash384::update(const uint8_t *const input, const size_t len);

This method is used to process the next **N** bytes (octets) of input data. It will update the hash computation state accordingly. The application needs to call this method repeatedly, on the *same* `MHash364` instance, until all input data has been processed.

*Parameters:*

* `const uint8_t *input`  
  Pointer to the input data to be processed by this operation. The input data needs to be located in one continuous block of memory. The given pointer specifies the *base address* of the input data, i.e. the address of the *first* byte (octet) to be processed.  
  *Note:* Formally, the input data is defined as a sequence of `uint8_t`, i.e. a sequence of bytes (octets). However, *any* suitable *byte*-based input data can be processed using the proper *typecast* operator.

* `size_t len`  
  The *length* of the input data to be processed, in bytes (octets). All memory addresses in the range from `input` up to and including `input+(len-1)` will be processed as input. Applications need to carefully check `len` to avoid buffer overruns!

### update() [2]

	void MHash384::update(const std::vector<uint8_t> &input, const size_t offset = 0, const size_t len = 0);

This method is used to process input a `std::vector<uint8_t>` as input. It will update the hash computation state accordingly. The application needs to call this method repeatedly, on the *same* `MHash364` instance, until all input data has been processed.

*Parameters:*

* `const std::vector<uint8_t> &input`  
  Reference to the `std::vector<uint8_t>` object to be processed as input. By default, all bytes (octets) in the vector will be processed. Optionally, a sub-range of the vector can be selected.

* `size_t offset`  
  Optional. Specifies the *zero-based* index of the *first* vector element to be processed. By default, processing starts at index **0**.

* `size_t len`  
  Optional. Specifies the number of vector elements to be processed. All elements from index `offset` up to and including index `offset+(len-1)` will be processed. By default, the whole vector is processed.

### update() [3]

	void MHash384::update(const std::string &input, const size_t offset = 0, const size_t len = 0);

This method is used to process input a `std::string` as input. It will update the hash computation state accordingly. The application needs to call this method repeatedly, on the *same* `MHash364` instance, until all input data has been processed.

*Parameters:*

* `const std::vector<uint8_t> &input`  
  Reference to the `std::string` object to be processed as input. By default, all characters (octets) in the string, excluding the terminating `NULL` character, will be processed. Optionally, a sub-range of the vector can be selected.

* `size_t offset`  
  Optional. Specifies the *zero-based* index of the *first* character in the string to be processed. By default, processing starts at index **0**.

* `size_t len`  
  Optional. Specifies the number of character to be processed. All characters from index `offset` up to and including index `offset+(len-1)` will be processed. By default, the whole string, excluding the terminating `NULL` character, is processed.

### finalize() [1]

	void MHash384::finalize(uint8_t *const output) const;

This method is used to finalize the hash computation and output the final digest (hash value). Typically, the application will call this method *once*, **after** all input data has been processed.

*Note:* The hash computation state is treated *read-only* by this method. This means that the application *may* call the method at any time to get an "intermediate" hash of all input bytes (octets) process *so far* and then continue to process more input bytes (octets).

*Parameters:*

* `uint8_t *output`
  Pointer to the memory block where the final digest (hash value) will be stored. This memory needs to be allocated by the calling application! The digest will be written to the memory addresses from `output` up to and including `output+(MHASH_384_LEN-1)`.

### finalize() [2]

	std::vector<uint8_t> MHash384::finalize(void) const;

This method is used to finalize the hash computation and output the final digest (hash value). Typically, the application will call this method *once*, **after** all input data has been processed.

*Note:* The hash computation state is treated *read-only* by this method. This means that the application *may* call the method at any time to get an "intermediate" hash of all input bytes (octets) process *so far* and then continue to process more input bytes (octets).

*Return value:*

* Returns a `std::vector<uint8_t>` containing the final digest (hash value). The size of the returned vector object will be exactly `MHASH_384_LEN` elements (octets).


# Supported Platforms

MHash-384 library should compile on any standard-compliant C/C++ compiler. In particular, the following platforms have been tested successfully:

* Microsoft Windows
    - Microsoft C/C++ Compiler, using Visual Studio 2010 or later
    - MinGW, using Mingw-w64 from [MSYS2](https://msys2.github.io/) project
* Intel C/C++ Compiler, version Version 15.0 (XE 2015) or later
* GNU/Linux, using GCC/G++, version 4.7 or later

# Language Bindings

While the MHash-384 library is primarily targeted for C/C++ applications, "native" ports of the library *are provided for a variety of programming languages. This allows for using the MHash-384 library in pretty much any scenario/environment.

## Microsoft.NET

Bindings of the MHash-384 library are provided for **Microsoft.NET**, in the form of the `MHashDotNet384.dll` assembly.

In order to use the MHash-384 library in your Microsoft.NET (e.g. C# or VB.NET) application, simply import and instantiate the provided `MHash384` class from the `MHashDotNet384` namespace:

	using MHashDotNet384;

	String ComputeHash(FileStream fs)
	{
		byte[] buffer = new byte[4096];
		MHash384 digest = new MHash384();
		while (true)
		{
			int count = fs.Read(buffer, 0, buffer.Length);
			if (count > 0)
			{
				digest.Update(buffer, 0, count);
				continue;
			}
			break;
		}
		return digest.ToString();
	}

### Prerequisites

In order to use the MHash-384 library in your Microsoft.NET (e.g. C# or VB.NET) application, a reference to the `MHashDotNet384.dll` assembly file **must** be added to the project.

## Java

Bindings of the MHash-384 library are provided for **Java**, in the form of the `MHashJava384.jar` library.

In order to use the MHash-384 library in your Java-based application, simply import and instantiate the provided `MHash384` class from the `com.muldersoft.mhash384` package:

	import com.muldersoft.mhash384.MHash384;
	
	String computeHash(final InputStream inputStream) throws IOException {
		final byte[] buffer = new byte[4096];
		final MHash384 mhash384 = new MHash384()
		int count;
		do {
			count = inputStream.read(buffer);
			if(count > 0) {
				mhash384.update(buffer, 0, count);
			}
		}
		while(count == buffer.length);
		return mhash384.digest().toString();
	}

### Prerequisites

In order to use the MHash-384 library in your Java project, the `MHashJava384.jar` **must** be added to the Java *build path*.

## Python

Bindings of the MHash-384 library are provided for **Python**, in the form of the `MHashPy384.py` module.

In order to use the MHash-384 library in your Python-based application, simply import and instantiate the provided `MHash384` class from the `MHashPy384` module:

	from MHashPy384 import MHash384
	
	mhash384 = MHash384()
	for chunk in read_chunks(fs):
		mhash384.update(chunk)
	print(binascii.hexlify(mhash384.digest()))

### Prerequisites

In order to use the MHash-384 library in your Python project, the `MHashPy384.py` **must** be in your Python *library path*.

The `MHashPy384.py` module requires the [***NumPy***](http://www.numpy.org/) package to be installed. See [*here*](https://scipy.org/install.html) for install instructions!

## Delphi

Bindings of the MHash-384 library are provided for **Delphi**, in the form of the `MHash384.pas` unit.

In order to use the MHash-384 library in your Delphi application, simply add the `MHash384` unit to the *uses* clause and instantiate the provided `TMHash384` class:

	uses
		{...}, MHash384;
	
	function ComputeHash(var inputFile: File): TByteArray;
	var
		digest: TMHash384;
		buffer: TByteArray;
		count: Integer;
	begin
		SetLength(buffer, 4096);
		digest := TMHash384.Create();
		try
			while not Eof(inputFile) do
			begin
				BlockRead(inputFile, buffer[0], Length(buffer), count);
				if count > 0 then
				begin
					digest.Update(buffer, 0, count);
				end;
			end;
			digest.Result(Result);
		finally
			digest.Destroy();
		end;
	end;

### Prerequisites

In order to use the MHash-384 library in your Delphi application, the Unit file 'MHash384.pas' must be added to the project. This unit contains the `TMHash384` convenience class and associated data types.


# Source Code

The MHash-384 source is available from the official [**git**](https://git-scm.com/) mirrors:

* `git clone https://github.com/lordmulder/mhash-384.git` [[Browse](https://github.com/lordmulder/mhash-384)]

* `git clone https://bitbucket.org/muldersoft/mhash-384.git` [[Browse](https://bitbucket.org/muldersoft/mhash-384)]

* `git clone https://git.assembla.com/mhash-384.git` [[Browse](https://www.assembla.com/spaces/mhash-384/git/source)]

* `git clone https://gitlab.com/lord_mulder/mhash-384.git` [[Browse](https://gitlab.com/lord_mulder/mhash-384)]


# Build Instructions

This section describes how to build the MHash-384 sample application. Please note that you do **not** need to "build" the library in order to use it in your own application.

* For supported versions of *Microsoft Visual Studio*, MHash-384 library ships with project/solution files, which will compile "out of the box".

* The *Intel C/C++ Compiler* integrates into Visual Studio, so simply select "Use Intel C++" from the project/solution menu.

* Optionally, the build script `Make.cmd` may be used to create ready-to-use deployment packages. Note, however, that it may be necessary to adjust the paths in the header section of the script!

* Finally, for the *GNU/Linux* and *MinGW/MSYS2* platforms, the MHash-384 library provides a Makefile (tested with GNU Make). Just run `make` from the MHash-384 directory.

## Influential Environment Variables

The following environment variables may effect the build process and need to be set carefully:

* **Java:**
    - `JAVA_HOME`: The *Java* "home" directory, should be pointing to JDK (*not* JRE) root directory
    - `ANT_HOME`: The *Apache Ant* "home" directory, should be pointing to root directory  

* **Python:**
    - `PYTHON_INC`: Directory to look for Python *include* files (typically `<PYTHON_INSTALL_PATH>/include`)
    - `PYTHON_LIB32`: Directory to look for 32-Bit (x86) Python *library* files (typically `<PYTHON_X86_PATH>/libs`)
    - `PYTHON_LIB64`: Directory to look for 64-Bit (x64) Python *library* files (typically `<PYTHON_X64_PATH>/libs`)  

* **Delphi:**
    - `DELPHI_PATH`: The *Borland Delphi* installation directory (for Windows only)  

* **Makefile:**
    - `CPU_TYPE`: Optimize binaries for the specified CPU type (defaults to "native"), see [`-march`](https://gcc.gnu.org/onlinedocs/gcc/x86-Options.html#x86-Options) for details!
    - `CPLUSPLUS`:  If set to `1`, build CLI front-end from *C++* sources, otherwise from *plain C* sources (defaults to `0`)
    - `NO_JAVA`: If set to `1`, the *Java* bindings are **not** built (defaults to `0`, i.e. *do* build)
    - `NO_PYTHON`: If set to `1`, the *Python* bindings are **not** built (defaults to `0`, i.e. *do* build)  

* **Windows:**
    - `MSVC_PATH`: *Microsoft Visual C++* installation directory
    - `PDOC_PATH`: *Pandoc v2.x* installation directory
    - `GIT2_PATH`: *Git for Windows* (formerly *MSYS Git*) installation directory  


# License

**Copyright(c) 2016-2018 LoRd_MuldeR &lt;mulder2@gmx.de&gt;, released under the MIT License.**  
**Check <http://muldersoft.com/> or <http://muldersoft.sourceforge.net/> for updates!**

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

## Version 1.1.0 [2017-12-22]

* Re-generated the XOR- and MIX-tables with higher hamming distance for increased hash quality

*  ***Note:*** This change, unfortunately, breaks compatibility with v1.0 hashes!

* All language bindings have been *replaced* by full ports of the library to the respective language

## Version 1.0.1 [2016-03-31]

* Added language bindings for *Java*.

* Added language bindings for *Microsoft.NET*.

* Added language bindings for *Python*.

* Added language bindings for *Delphi*.

## Version 1.0.0 [2016-03-03]

* First public release.

&nbsp;

[■](https://www.youtube.com/watch?v=dng06ZqI4Ss)
