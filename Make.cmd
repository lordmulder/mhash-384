@echo off
setlocal enabledelayedexpansion
cd /d "%~dp0"

REM ///////////////////////////////////////////////////////////////////////////
REM // Setup environment
REM ///////////////////////////////////////////////////////////////////////////

REM Build Tool-Chain
set "MSVC_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build"
set "GIT2_PATH=C:\Program Files\Git\bin"
set "JAVA_HOME=C:\Program Files\Java\jdk1.8.0_152"
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
	for %%j in (native,library,wrapper,example) do (
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

call "%MSVC_PATH%\vcvarsall.bat" x86

set "MSVC_PROJECTS=MHashLib.sln"
set "MSVC_CONFIGS=Release,Release_SSE2,Release_AVX"
for %%q in (%MSVC_PROJECTS%) do (
	for %%p in (x86,x64) do (
		for %%c in (%MSVC_CONFIGS%) do (
			"%~dp0\tools\cecho.exe" CYAN "\n----[ %%~nq (%%p,%%c) ]----\n"
			MSBuild.exe /property:Platform=%%p /property:Configuration=%%c /target:Clean   "%~dp0\%%~q"
			if not "!ERRORLEVEL!"=="0" goto BuildHasFailed
			MSBuild.exe /property:Platform=%%p /property:Configuration=%%c /target:Rebuild "%~dp0\%%~q"
			if not "!ERRORLEVEL!"=="0" goto BuildHasFailed
		)
	)
)

set "DOTNET_PROJECTS=bindings\Microsoft.NET\MHashDotNet384.sln"
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
set "OUT_PATH_BIN_X86=%~dp0\out\mhash_384.%ISO_DATE%%REVISON%.msvc-x86.zip"
set "OUT_PATH_BIN_X64=%~dp0\out\mhash_384.%ISO_DATE%%REVISON%.msvc-x64.zip"
set "OUT_PATH_NET_GEN=%~dp0\out\mhash_384.%ISO_DATE%%REVISON%.dotnet.zip"
set "OUT_PATH_JNI_GEN=%~dp0\out\mhash_384.%ISO_DATE%%REVISON%.java.zip"
set "OUT_PATH_PYC_GEN=%~dp0\out\mhash_384.%ISO_DATE%%REVISON%.python.zip"
set "OUT_PATH_PAS_X86=%~dp0\out\mhash_384.%ISO_DATE%%REVISON%.delphi-x86.zip"
set "OUT_PATH_SRC_GEN=%~dp0\out\mhash_384.%ISO_DATE%%REVISON%.sources.tar.gz"

set /a COUNTER=COUNTER+1
set REVISON=.update-%COUNTER%

if exist "%OUT_PATH_BIN_X86%" goto GenerateOutfileNameNext
if exist "%OUT_PATH_BIN_X64%" goto GenerateOutfileNameNext
if exist "%OUT_PATH_NET_GEN%" goto GenerateOutfileNameNext
if exist "%OUT_PATH_JNI_GEN%" goto GenerateOutfileNameNext
if exist "%OUT_PATH_PYC_X86%" goto GenerateOutfileNameNext
if exist "%OUT_PATH_PYC_X64%" goto GenerateOutfileNameNext
if exist "%OUT_PATH_PAS_X86%" goto GenerateOutfileNameNext
if exist "%OUT_PATH_SRC_GEN%" goto GenerateOutfileNameNext


REM ///////////////////////////////////////////////////////////////////////////
REM // Build ZIP package
REM ///////////////////////////////////////////////////////////////////////////

"%~dp0\tools\cecho.exe" YELLOW "\n========[ PACKAGING ]========\n"

"%~dp0\tools\zip.exe" -j -9 -z "%OUT_PATH_BIN_X86%" "%~dp0\include\mhash_384.h" "%~dp0\README.html" "%~dp0\COPYING.txt" "%~dp0\bin\Win32\Release\mhash_384.x86-i386.exe" "%~dp0\bin\Win32\Release_SSE2\mhash_384.x86-sse2.exe" "%~dp0\bin\Win32\Release_AVX\mhash_384.x86-avx.exe" < "%~dp0\COPYING.txt"
"%~dp0\tools\zip.exe" -j -9 -z "%OUT_PATH_BIN_X64%" "%~dp0\include\mhash_384.h" "%~dp0\README.html" "%~dp0\COPYING.txt" "%~dp0\bin\x64\Release\mhash_384.x64-sse2.exe" "%~dp0\bin\x64\Release_AVX\mhash_384.x64-avx.exe" < "%~dp0\COPYING.txt"

"%~dp0\tools\zip.exe" -j -9 -z "%OUT_PATH_NET_GEN%" "%~dp0\bindings\Microsoft.NET\library\bin\Release\MHashDotNet384.dll" "%~dp0\bindings\Microsoft.NET\example\bin\Release\MHashDotNet384.Example.exe" "%~dp0\README.html" "%~dp0\COPYING.txt" < "%~dp0\COPYING.txt"

"%~dp0\tools\zip.exe" -j -9 -z "%OUT_PATH_JNI_GEN%" "%~dp0\bindings\Java\library\out\MHashJava384.jar" "%~dp0\bindings\Java\example\out\MHashJava384-Example.jar" "%~dp0\README.html" "%~dp0\COPYING.txt" < "%~dp0\COPYING.txt"

"%~dp0\tools\zip.exe" -j -9 -z "%OUT_PATH_PYC_GEN%" "%~dp0\bindings\Python\library\MHashPy384.py" "%~dp0\bindings\Python\example\MHashPy384_Example.py" "%~dp0\README.html" "%~dp0\COPYING.txt" < "%~dp0\COPYING.txt"

"%~dp0\tools\zip.exe" -j -9 -z "%OUT_PATH_PAS_X86%" "%~dp0\bindings\Delphi\library\MHash384.pas" "%~dp0\bindings\Delphi\example\bin\Example.exe" "%~dp0\README.html" "%~dp0\COPYING.txt" < "%~dp0\COPYING.txt"

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