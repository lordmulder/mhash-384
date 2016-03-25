@echo off
setlocal enabledelayedexpansion
cd /d "%~dp0"

REM ///////////////////////////////////////////////////////////////////////////
REM // Setup environment
REM ///////////////////////////////////////////////////////////////////////////

set "MSVC_PATH=C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC"
set "PDOC_PATH=C:\Program Files (x86)\Pandoc"
set "GIT2_PATH=C:\Program Files\Git\bin"

REM Java Paths
set "JDK_HOME=C:\Program Files\Java\jdk1.8.0_77"
set "ANT_HOME=C:\Eclipse\apache-ant"

REM Python Paths
set "PYTHON_HOME_X86=C:\Program Files (x86)\Python35-32"
set "PYTHON_HOME_X64=C:\Program Files\Python35"


REM ///////////////////////////////////////////////////////////////////////////
REM // Check paths
REM ///////////////////////////////////////////////////////////////////////////

if not exist "%MSVC_PATH%\vcvarsall.bat" (
	"%~dp0\tools\cecho.exe" RED "\nMSVC not found.\n%MSVC_PATH:\=\\%\\vcvarsall.bat\n"
	pause & goto:eof
)

if not exist "%PDOC_PATH%\pandoc.exe" (
	"%~dp0\tools\cecho.exe" RED "\nPandoc not found.\n%PDOC_PATH:\=\\%\\pandoc.exe\n"
	pause & goto:eof
)

if not exist "%GIT2_PATH%\git.exe" (
	"%~dp0\tools\cecho.exe" RED "\nGIT not found.\n%GIT2_PATH:\=\\%\\git.exe\n"
	pause & goto:eof
)

if not exist "%JDK_HOME%\lib\tools.jar" (
	"%~dp0\tools\cecho.exe" RED "\nJava not found.\n%JDK_HOME:\=\\%\\lib\\tools.jar\n"
	pause & goto:eof
)

if not exist "%JDK_HOME%\bin\javac.exe" (
	"%~dp0\tools\cecho.exe" RED "\nJava not found.\n%JDK_HOME:\=\\%\\bin\\javac.exe\n"
	pause & goto:eof
)

if not exist "%ANT_HOME%\bin\ant.bat" (
	"%~dp0\tools\cecho.exe" RED "\nAnt not found.\n%ANT_HOME:\=\\%\\bin\\ant.cmd\n"
	pause & goto:eof
)

if not exist "%ANT_HOME%\lib\ant.jar" (
	"%~dp0\tools\cecho.exe" RED "\nAnt not found.\n%ANT_HOME:\=\\%\\lib\\ant.jar\n"
	pause & goto:eof
)

if not exist "%PYTHON_HOME_X86%\include\Python.h" (
	"%~dp0\tools\cecho.exe" RED "\nPython-x86 not found.\n%PYTHON_HOME_X86:\=\\%\\include\\Python.h\n"
	pause & goto:eof
)

if not exist "%PYTHON_HOME_X64%\include\Python.h" (
	"%~dp0\tools\cecho.exe" RED "\nPython-x64 not found.\n%PYTHON_HOME_X64:\=\\%\\include\\Python.h\n"
	pause & goto:eof
)


REM ///////////////////////////////////////////////////////////////////////////
REM // Get current date and time (in ISO format)
REM ///////////////////////////////////////////////////////////////////////////

set "ISO_DATE="
set "ISO_TIME="
for /F "tokens=1,2 delims=:" %%a in ('"%~dp0\tools\date.exe" +ISODATE:%%Y-%%m-%%d') do (
	if "%%a"=="ISODATE" set "ISO_DATE=%%b"
)
for /F "tokens=1,2,3,4 delims=:" %%a in ('"%~dp0\tools\date.exe" +ISOTIME:%%T') do (
	if "%%a"=="ISOTIME" set "ISO_TIME=%%b:%%c:%%d"
)


REM ///////////////////////////////////////////////////////////////////////////
REM // Clean up temp files
REM ///////////////////////////////////////////////////////////////////////////

"%~dp0\tools\cecho.exe" YELLOW "\n========[ CLEAN UP ]========\n"

for %%i in (bin,obj) do (
	del /Q /S /F "%~dp0\%%i\*.*"
)

for %%i in (bin,MHashDotNet384.Example\obj,MHashDotNet384.Wrapper\obj) do (
	del /Q /S /F "%~dp0\bindings\Microsoft.NET\%%i\*.*"
)

for %%i in (native\obj,native\bin,warapper\bin,warapper\out) do (
	del /Q /S /F "%~dp0\bindings\Java\%%i\*.*"
)

for %%i in (wrapper\obj,wrapper\out) do (
	del /Q /S /F "%~dp0\bindings\Python\%%i\*.*"
)


REM ///////////////////////////////////////////////////////////////////////////
REM // Build the binaries
REM ///////////////////////////////////////////////////////////////////////////

"%~dp0\tools\cecho.exe" YELLOW "\n========[ COMPILE ]========"

set "JAVA_HOME=%JDK_HOME%"
set "ANT_HOME=%ANT_HOME%"
call "%MSVC_PATH%\vcvarsall.bat"

set "MSVC_PROJECTS=MHashLib.sln"
set "MSVC_PROJECTS=%MSVC_PROJECTS%,bindings\Microsoft.NET\MHashDotNet384.sln"
set "MSVC_PROJECTS=%MSVC_PROJECTS%,bindings\Java\native\MHashJava384.sln"
set "MSVC_PROJECTS=%MSVC_PROJECTS%,bindings\Python\wrapper\MHashPy384.sln"

for %%q in (%MSVC_PROJECTS%) do (
	for %%p in (x86,x64) do (
		"%~dp0\tools\cecho.exe" CYAN "\n----[ %%~nq (%%~p) ]----\n"
		MSBuild.exe /property:Platform=%%p /property:Configuration=Release /target:Clean   "%~dp0\%%~q"
		if not "!ERRORLEVEL!"=="0" goto BuildHasFailed
		MSBuild.exe /property:Platform=%%p /property:Configuration=Release /target:Rebuild "%~dp0\%%~q"
		if not "!ERRORLEVEL!"=="0" goto BuildHasFailed
	)
)

for %%q in (wrapper,example) do (
	"%~dp0\tools\cecho.exe" CYAN "\n----[ %%~nq ]----\n"
	pushd "%~dp0\bindings\Java\%%~q"
	call "%ANT_HOME%\bin\ant.bat" clean jar
	if not "!ERRORLEVEL!"=="0" goto BuildHasFailed
	popd
)


REM ///////////////////////////////////////////////////////////////////////////
REM // Generate Docs
REM ///////////////////////////////////////////////////////////////////////////

"%PDOC_PATH%\pandoc.exe" --from markdown --to html5 --toc -N --standalone --output "%~dp0\README.html" "%~dp0\README.md"
if not "%ERRORLEVEL%"=="0" goto BuildHasFailed


REM ///////////////////////////////////////////////////////////////////////////
REM // Generate output name
REM ///////////////////////////////////////////////////////////////////////////

mkdir "%~dp0\out" 2> NUL

set COUNTER=
set REVISON=

:GenerateOutfileNameNext
set "OUT_PATH_BIN_X86=%~dp0\out\mhash_384.%ISO_DATE%%REVISON%.bin-msvc-x86.zip"
set "OUT_PATH_BIN_X64=%~dp0\out\mhash_384.%ISO_DATE%%REVISON%.bin-msvc-x64.zip"
set "OUT_PATH_NET_X86=%~dp0\out\mhash_384.%ISO_DATE%%REVISON%.dotnet-x86.zip"
set "OUT_PATH_NET_X64=%~dp0\out\mhash_384.%ISO_DATE%%REVISON%.dotnet-x64.zip"
set "OUT_PATH_JNI_X86=%~dp0\out\mhash_384.%ISO_DATE%%REVISON%.java-win-x86.zip"
set "OUT_PATH_JNI_X64=%~dp0\out\mhash_384.%ISO_DATE%%REVISON%.java-win-x64.zip"
set "OUT_PATH_PYC_X86=%~dp0\out\mhash_384.%ISO_DATE%%REVISON%.python-win-x86.zip"
set "OUT_PATH_PYC_X64=%~dp0\out\mhash_384.%ISO_DATE%%REVISON%.python-win-x64.zip"
set "OUT_PATH_SRC_GEN=%~dp0\out\mhash_384.%ISO_DATE%%REVISON%.sources.tar.gz"

set /a COUNTER=COUNTER+1
set REVISON=.update-%COUNTER%

if exist "%OUT_PATH_BIN_X86%" goto GenerateOutfileNameNext
if exist "%OUT_PATH_BIN_X64%" goto GenerateOutfileNameNext
if exist "%OUT_PATH_NET_X86%" goto GenerateOutfileNameNext
if exist "%OUT_PATH_NET_X64%" goto GenerateOutfileNameNext
if exist "%OUT_PATH_JNI_X86%" goto GenerateOutfileNameNext
if exist "%OUT_PATH_JNI_X64%" goto GenerateOutfileNameNext
if exist "%OUT_PATH_PYC_X86%" goto GenerateOutfileNameNext
if exist "%OUT_PATH_PYC_X64%" goto GenerateOutfileNameNext
if exist "%OUT_PATH_SRC_GEN%" goto GenerateOutfileNameNext


REM ///////////////////////////////////////////////////////////////////////////
REM // Build ZIP package
REM ///////////////////////////////////////////////////////////////////////////

"%~dp0\tools\cecho.exe" YELLOW "\n========[ PACKAGING ]========\n"

"%~dp0\tools\zip.exe" -j -9 -z "%OUT_PATH_BIN_X86%" "%~dp0\bin\Win32\Release\mhash_384.x86.exe" "%~dp0\README.html" "%~dp0\COPYING.txt" < "%~dp0\COPYING.txt"
"%~dp0\tools\zip.exe" -j -9 -z "%OUT_PATH_BIN_X64%" "%~dp0\bin\x64\.\Release\mhash_384.x64.exe" "%~dp0\README.html" "%~dp0\COPYING.txt" < "%~dp0\COPYING.txt"

"%~dp0\tools\zip.exe" -j -9 -z "%OUT_PATH_NET_X86%" "%~dp0\bindings\Microsoft.NET\bin\x86\Release\MHashDotNet384.x86.dll" "%~dp0\bindings\Microsoft.NET\bin\x86\Release\MHashDotNet384.Example.exe" "%~dp0\README.html" "%~dp0\COPYING.txt" < "%~dp0\COPYING.txt"
"%~dp0\tools\zip.exe" -j -9 -z "%OUT_PATH_NET_X64%" "%~dp0\bindings\Microsoft.NET\bin\x64\Release\MHashDotNet384.x64.dll" "%~dp0\bindings\Microsoft.NET\bin\x64\Release\MHashDotNet384.Example.exe" "%~dp0\README.html" "%~dp0\COPYING.txt" < "%~dp0\COPYING.txt"

"%~dp0\tools\zip.exe" -j -9 -z "%OUT_PATH_JNI_X86%" "%~dp0\bindings\Java\native\bin\x86\Release\MHashJava384.x86.dll" "%~dp0\bindings\Java\wrapper\out\MHashJava384-Wrapper.jar" "%~dp0\bindings\Java\example\out\MHashJava384-Example.jar" "%~dp0\README.html" "%~dp0\COPYING.txt" < "%~dp0\COPYING.txt"
"%~dp0\tools\zip.exe" -j -9 -z "%OUT_PATH_JNI_X64%" "%~dp0\bindings\Java\native\bin\x64\Release\MHashJava384.x64.dll" "%~dp0\bindings\Java\wrapper\out\MHashJava384-Wrapper.jar" "%~dp0\bindings\Java\example\out\MHashJava384-Example.jar" "%~dp0\README.html" "%~dp0\COPYING.txt" < "%~dp0\COPYING.txt"

"%~dp0\tools\zip.exe" -j -9 -z "%OUT_PATH_PYC_X86%" "%~dp0\bindings\Python\wrapper\bin\x86\Release\MHashPy384.x86.pyd" "%~dp0\README.html" "%~dp0\COPYING.txt" < "%~dp0\COPYING.txt"
"%~dp0\tools\zip.exe" -j -9 -z "%OUT_PATH_PYC_X64%" "%~dp0\bindings\Python\wrapper\bin\x64\Release\MHashPy384.x64.pyd" "%~dp0\README.html" "%~dp0\COPYING.txt" < "%~dp0\COPYING.txt"

"%GIT2_PATH%\git.exe" archive --format tar.gz -9 --verbose --output "%OUT_PATH_SRC_GEN%" HEAD


REM ///////////////////////////////////////////////////////////////////////////
REM // Completed
REM ///////////////////////////////////////////////////////////////////////////

"%~dp0\tools\cecho.exe" GREEN "\nBUILD COMPLETED.\n"
pause
goto:eof


REM ///////////////////////////////////////////////////////////////////////////
REM // Failed
REM ///////////////////////////////////////////////////////////////////////////

:BuildHasFailed
"%~dp0\tools\cecho.exe" RED "\nBUILD HAS FAILED.\n"
pause
goto:eof