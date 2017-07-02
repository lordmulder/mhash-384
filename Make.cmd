@echo off
setlocal enabledelayedexpansion
cd /d "%~dp0"

REM ///////////////////////////////////////////////////////////////////////////
REM // Setup environment
REM ///////////////////////////////////////////////////////////////////////////

REM Build Tool-Chain
set "MSVC_PATH=C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC"
set "GIT2_PATH=C:\Program Files\Git\bin"
set "JAVA_HOME=C:\Program Files\Java\jdk1.8.0_131"
set "DELPHI_PATH=C:\Program Files (x86)\Borland\Delphi7"

REM Prerequisites
set "PDOC_PATH=%~dp0\..\Prerequisites\Pandoc"
set "ANT_HOME=%~dp0\..\Prerequisites\Ant"
set "PYTHON_HOME_INC=%~dp0\..\Prerequisites\Python3\include"
set "PYTHON_HOME_LIB32=%~dp0\..\Prerequisites\Python3\lib\Win32"
set "PYTHON_HOME_LIB64=%~dp0\..\Prerequisites\Python3\lib\x64"


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

if not exist "%JAVA_HOME%\lib\tools.jar" (
	"%~dp0\tools\cecho.exe" RED "\nJava not found.\n%JAVA_HOME:\=\\%\\lib\\tools.jar\n"
	pause & goto:eof
)

if not exist "%JAVA_HOME%\bin\javac.exe" (
	"%~dp0\tools\cecho.exe" RED "\nJava not found.\n%JAVA_HOME:\=\\%\\bin\\javac.exe\n"
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

if not exist "%PYTHON_HOME_INC%\Python.h" (
	"%~dp0\tools\cecho.exe" RED "\nPython includes not found.\n%PYTHON_HOME_INC:\=\\%\\Python.h\n"
	pause & goto:eof
)

if not exist "%PYTHON_HOME_LIB32%\python3.lib" (
	"%~dp0\tools\cecho.exe" RED "\nPython-x86 not found.\n%PYTHON_HOME_LIB32:\=\\%\\python3.lib\n"
	pause & goto:eof
)

if not exist "%PYTHON_HOME_LIB64%\python3.lib" (
	"%~dp0\tools\cecho.exe" RED "\nPython-x64 not found.\n%PYTHON_HOME_LIB64:\=\\%\\python3.lib\n"
	pause & goto:eof
)

if not exist "%DELPHI_PATH%\bin\dcc32.exe" (
	"%~dp0\tools\cecho.exe" RED "\nDelphi not found.\n%DELPHI_PATH:\=\\%\\bin\\dcc32.exe\n"
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
	del /Q /S /F "%~dp0\%%i\*.*"  2> NUL
)

for %%i in (Microsoft.NET,Java,Delphi,Python) do (
	for %%j in (native,wrapper,example) do (
		for %%k in (bin,obj,out) do (
			del /Q /S /F "%~dp0\bindings\%%i\%%j\%%k\*.*" 2> NUL
		)
	)
)

for %%i in (exe,dcu) do (
	del /Q /S /F "%~dp0\bindings\Delphi\*.%%i" 2> NUL
)


REM ///////////////////////////////////////////////////////////////////////////
REM // Build the binaries
REM ///////////////////////////////////////////////////////////////////////////

"%~dp0\tools\cecho.exe" YELLOW "\n========[ COMPILE ]========"

call "%MSVC_PATH%\vcvarsall.bat"

set "MSVC_PROJECTS=MHashLib.sln"
::set "MSVC_PROJECTS=%MSVC_PROJECTS%,bindings\Python\native\MHashPy384_Native.sln"
set "MSVC_PROJECTS=%MSVC_PROJECTS%,bindings\Delphi\native\MHashDelphi384.sln"

set "DOTNET_PROJECTS=bindings\Microsoft.NET\MHashDotNet384.sln"

for %%q in (%MSVC_PROJECTS%) do (
	for %%p in (x86,x64) do (
		"%~dp0\tools\cecho.exe" CYAN "\n----[ %%~nq (%%~p) ]----\n"
		MSBuild.exe /property:Platform=%%p /property:Configuration=Release /target:Clean   "%~dp0\%%~q"
		if not "!ERRORLEVEL!"=="0" goto BuildHasFailed
		MSBuild.exe /property:Platform=%%p /property:Configuration=Release /target:Rebuild "%~dp0\%%~q"
		if not "!ERRORLEVEL!"=="0" goto BuildHasFailed
	)
)

for %%q in (%DOTNET_PROJECTS%) do (
	"%~dp0\tools\cecho.exe" CYAN "\n----[ %%~nq (%%~p) ]----\n"
	MSBuild.exe /property:Platform="Any CPU" /property:Configuration=Release /target:Clean   "%~dp0\%%~q"
	if not "!ERRORLEVEL!"=="0" goto BuildHasFailed
	MSBuild.exe /property:Platform="Any CPU" /property:Configuration=Release /target:Rebuild "%~dp0\%%~q"
	if not "!ERRORLEVEL!"=="0" goto BuildHasFailed
)


for %%q in (example) do (
	echo pushd "%~dp0\bindings\Delphi\%%~q"
	pushd "%~dp0\bindings\Delphi\%%~q"
	for %%p in (*.dpr) do (
		"%~dp0\tools\cecho.exe" CYAN "\n----[ %%~np ]----\n"
		"%DELPHI_PATH%\bin\dcc32.exe" -B -CG -$O+ -E"bin" -N"obj" "%%~nxp"
		if not "!ERRORLEVEL!"=="0" goto BuildHasFailed
	)
	popd
)

for %%q in (library,example) do (
	"%~dp0\tools\cecho.exe" CYAN "\n----[ %%~nq ]----\n"
	pushd "%~dp0\bindings\Java\%%~q"
	call "%ANT_HOME%\bin\ant.bat" clean jar
	if not "!ERRORLEVEL!"=="0" goto BuildHasFailed
	popd
)


REM ///////////////////////////////////////////////////////////////////////////
REM // Generate Docs
REM ///////////////////////////////////////////////////////////////////////////

"%PDOC_PATH%\pandoc.exe" --from markdown --to html5 --toc -N --standalone -H "%~dp0\etc\css\style.inc" --output "%~dp0\README.html" "%~dp0\README.md"
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
set "OUT_PATH_PAS_X86=%~dp0\out\mhash_384.%ISO_DATE%%REVISON%.delphi-win-x86.zip"
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
if exist "%OUT_PATH_PAS_X86%" goto GenerateOutfileNameNext
if exist "%OUT_PATH_SRC_GEN%" goto GenerateOutfileNameNext


REM ///////////////////////////////////////////////////////////////////////////
REM // Build ZIP package
REM ///////////////////////////////////////////////////////////////////////////

"%~dp0\tools\cecho.exe" YELLOW "\n========[ PACKAGING ]========\n"

"%~dp0\tools\zip.exe" -j -9 -z "%OUT_PATH_BIN_X86%" "%~dp0\bin\Win32\Release\mhash_384.x86.exe" "%~dp0\README.html" "%~dp0\COPYING.txt" < "%~dp0\COPYING.txt"
"%~dp0\tools\zip.exe" -j -9 -z "%OUT_PATH_BIN_X64%" "%~dp0\bin\x64\.\Release\mhash_384.x64.exe" "%~dp0\README.html" "%~dp0\COPYING.txt" < "%~dp0\COPYING.txt"

"%~dp0\tools\zip.exe" -j -9 -z "%OUT_PATH_NET_X86%" "%~dp0\bindings\Microsoft.NET\wrapper\bin\x86\Release\MHashDotNet384.x86.dll" "%~dp0\bindings\Microsoft.NET\example\bin\x86\Release\MHashDotNet384.exe" "%~dp0\README.html" "%~dp0\COPYING.txt" < "%~dp0\COPYING.txt"
"%~dp0\tools\zip.exe" -j -9 -z "%OUT_PATH_NET_X64%" "%~dp0\bindings\Microsoft.NET\wrapper\bin\x64\Release\MHashDotNet384.x64.dll" "%~dp0\bindings\Microsoft.NET\example\bin\x64\Release\MHashDotNet384.exe" "%~dp0\README.html" "%~dp0\COPYING.txt" < "%~dp0\COPYING.txt"

"%~dp0\tools\zip.exe" -j -9 -z "%OUT_PATH_JNI_X86%" "%~dp0\bindings\Java\native\bin\x86\Release\MHashJava384.x86.dll" "%~dp0\bindings\Java\wrapper\out\MHashJava384-Wrapper.jar" "%~dp0\bindings\Java\example\out\MHashJava384-Example.jar" "%~dp0\README.html" "%~dp0\COPYING.txt" < "%~dp0\COPYING.txt"
"%~dp0\tools\zip.exe" -j -9 -z "%OUT_PATH_JNI_X64%" "%~dp0\bindings\Java\native\bin\x64\Release\MHashJava384.x64.dll" "%~dp0\bindings\Java\wrapper\out\MHashJava384-Wrapper.jar" "%~dp0\bindings\Java\example\out\MHashJava384-Example.jar" "%~dp0\README.html" "%~dp0\COPYING.txt" < "%~dp0\COPYING.txt"

"%~dp0\tools\zip.exe" -j -9 -z "%OUT_PATH_PYC_X86%" "%~dp0\bindings\Python\native\bin\x86\Release\MHashPy384_Native.x86.pyd" "%~dp0\bindings\Python\wrapper\MHashPy384_Wrapper.py" "%~dp0\bindings\Python\wrapper\mhash.pth" "%~dp0\bindings\Python\example\MHashPy384_Example.py" "%~dp0\README.html" "%~dp0\COPYING.txt" < "%~dp0\COPYING.txt"
"%~dp0\tools\zip.exe" -j -9 -z "%OUT_PATH_PYC_X64%" "%~dp0\bindings\Python\native\bin\x64\Release\MHashPy384_Native.x64.pyd" "%~dp0\bindings\Python\wrapper\MHashPy384_Wrapper.py" "%~dp0\bindings\Python\wrapper\mhash.pth" "%~dp0\bindings\Python\example\MHashPy384_Example.py" "%~dp0\README.html" "%~dp0\COPYING.txt" < "%~dp0\COPYING.txt"

"%~dp0\tools\zip.exe" -j -9 -z "%OUT_PATH_PAS_X86%" "%~dp0\bindings\Delphi\native\bin\x86\Release\MHashDelphi384.x86.dll" "%~dp0\bindings\Delphi\wrapper\MHash384.pas" "%~dp0\bindings\Delphi\example\bin\Example.exe" "%~dp0\README.html" "%~dp0\COPYING.txt" < "%~dp0\COPYING.txt"

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