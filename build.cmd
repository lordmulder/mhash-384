@echo off
setlocal enabledelayedexpansion

REM =========================================================================
REM READ CONFIGURATION
REM =========================================================================

if not exist "%~dpn0.ini" (
	echo Configuration file "%~dpn0.ini" not found^^!
	pause
	goto:eof
)

set "CONFVARS_TMP=%TMP%\~%RANDOM%$%RANDOM%$%RANDOM%.cmd"
"%~dp0\etc\utils\inifile.exe" "%~dpn0.ini" [PATHS] > "%CONFVARS_TMP%"

call "%CONFVARS_TMP%"
del  "%CONFVARS_TMP%"

REM =========================================================================
REM CHECK PATHS
REM =========================================================================

if not exist "%MSVC_PATH%\vcvarsall.bat" (
	if not exist "%MSVC_PATH%\Auxiliary\Build\vcvarsall.bat" (
		echo MSVC not found. Please check MSVC_PATH and try again^^!
		pause
		goto:eof
	)
)

if not exist "%GIT_PATH%\bin\git.exe" (
	echo Git not found. Please check GIT_PATH and try again^^!
	pause
	goto:eof
)

if not exist "%PANDOC_PATH%\pandoc.exe" (
	echo Pandoc not found. Please check PANDOC_PATH and try again^^!
	pause
	goto:eof
)


if not exist "%JAVA_HOME%\bin\java.exe" (
	echo Java not found. Please check JAVA_HOME and try again^^!
	pause
	goto:eof
)

if not exist "%ANT_HOME%\bin\ant.bat" (
	echo Ant not found. Please check ANT_HOME and try again^^!
	pause
	goto:eof
)

if not exist "%PYTHONHOME%\python.exe" (
	echo Python not found. Please check PYTHONHOME and try again^^!
	pause
	goto:eof
)

if not exist "%DELPHI_HOME%\bin\dcc32.exe" (
	echo Delphi not found. Please check DELPHI_HOME and try again^^!
	pause
	goto:eof
)

REM =========================================================================
REM SET UP ENVIRONMENT
REM =========================================================================

set "PATH=%GIT_PATH%\bin;%PATH%"
set VCINSTALLDIR=

if exist "%MSVC_PATH%\Auxiliary\Build\vcvarsall.bat" (
	call "%MSVC_PATH%\Auxiliary\Build\vcvarsall.bat" x86
	goto:msvc_initialized
)

if exist "%MSVC_PATH%\vcvarsall.bat" (
	call "%MSVC_PATH%\vcvarsall.bat" x86
	goto:msvc_initialized
)

:msvc_initialized

if "%VCINSTALLDIR%"=="" (
	echo Error: Failed to set up MSVC environment^^!
	pause
	goto:eof
)

REM =========================================================================
REM DETECT BUILD DATE
REM =========================================================================

set "ISO_DATE="
set "ISO_TIME="

for /F "tokens=1,* delims=:" %%a in ('"%~dp0\etc\utils\date.exe" +ISODATE:%%Y-%%m-%%d') do (
	if "%%a"=="ISODATE" set "ISO_DATE=%%b"
)

for /F "tokens=1,* delims=:" %%a in ('"%~dp0\etc\utils\date.exe" +ISOTIME:%%H:%%M:%%S') do (
	if "%%a"=="ISOTIME" set "ISO_TIME=%%b"
)

if "%ISO_DATE%"=="" goto:buildate_error
if "%ISO_TIME%"=="" goto:buildate_error
goto:buildate_complete

:buildate_error
echo Error: Failed to determine build date^^!
pause
goto:eof

:buildate_complete

REM =========================================================================
REM READ VERSION NUMBER
REM =========================================================================

set MHASH384_VERSION_MAJOR=
set MHASH384_VERSION_MINOR=
set MHASH384_VERSION_PATCH=

for /f "tokens=1,2,3" %%u in (%~dp0\libmhash384\src\mhash384.cpp) do (
	if "%%~u"=="#define" (
		if "%%~v"=="MHASH384_VERSION_MAJOR" set "MHASH384_VERSION_MAJOR=%%~w"
		if "%%~v"=="MHASH384_VERSION_MINOR" set "MHASH384_VERSION_MINOR=%%~w"
		if "%%~v"=="MHASH384_VERSION_PATCH" set "MHASH384_VERSION_PATCH=%%~w"
	)
)

if "%MHASH384_VERSION_MAJOR%"=="" goto:version_error
if "%MHASH384_VERSION_MINOR%"=="" goto:version_error
if "%MHASH384_VERSION_PATCH%"=="" goto:version_error
goto:version_complete

:version_error
echo Error: Failed to determine library version from source code^^!
pause
goto:eof

:version_complete

if %MHASH384_VERSION_MINOR% LSS 10 (
	set "MHASH384_VERSION_STR=%MHASH384_VERSION_MAJOR%.0%MHASH384_VERSION_MINOR%-%MHASH384_VERSION_PATCH%"
) else (
	set "MHASH384_VERSION_STR=%MHASH384_VERSION_MAJOR%.%MHASH384_VERSION_MINOR%-%MHASH384_VERSION_PATCH%"
)

REM =========================================================================
REM REMOVE EXISTING BINARIES
REM =========================================================================

for %%p in (bin,obj) do (
	if exist "%~dp0\%%p\" rmdir /S /Q "%~dp0\%%p"
)

for %%p in (frontend,libmhash384,ports) do (
	"%GIT_PATH%\bin\git.exe" clean -fdx "%~dp0\%%p"
)

REM =========================================================================
REM BUILD BINARIES
REM =========================================================================

set "PACK_PATH=%TMP%\~%RANDOM%$%RANDOM%.tmp"
mkdir "%PACK_PATH%"

echo.
echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
echo BUILD BINARIES
echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
echo.

for %%p in (Win32,x64) do (
	for %%t in (Clean,Rebuild) do (
		MSBuild.exe /property:Configuration=Release /property:Platform=%%p /target:%%t /verbosity:normal "%~dp0\MHash384.sln"
		if not "!ERRORLEVEL!"=="0" (
			echo Error: Build has failed^^!
			pause
			goto:eof
		)
	)
)

"%~dp0\etc\utils\rchhdrrsr.exe" "%~dp0\bin\Win32\Release\*.exe"
"%~dp0\etc\utils\rchhdrrsr.exe" "%~dp0\bin\.\x64\Release\*.exe"

for %%p in (Win32,x64) do (
	for %%t in (Clean,Rebuild) do (
		MSBuild.exe /property:Configuration=Release /property:Platform=%%p /target:%%t /verbosity:normal "%~dp0\MHash384.sln"
		if not "!ERRORLEVEL!"=="0" (
			echo Error: Build has failed^^!
			pause
			goto:eof
		)
	)
)

mkdir "%PACK_PATH%\win-x86"
mkdir "%PACK_PATH%\win-x64"

"%~dp0\bin\Win32\Release\MHash384.exe" --version > "%PACK_PATH%\win-x86\BUILD_TAG.txt"
"%~dp0\bin\.\x64\Release\MHash384.exe" --version > "%PACK_PATH%\win-x64\BUILD_TAG.txt"

copy /Y /B "%~dp0\bin\Win32\Release\MHash384.exe" "%PACK_PATH%\win-x86"
copy /Y /B "%~dp0\bin\.\x64\Release\MHash384.exe" "%PACK_PATH%\win-x64"

REM -----------------------------------------------------------
REM C# PORT
REM -----------------------------------------------------------

pushd "%~dp0\ports\dotnet"

for %%t in (Clean,Rebuild) do (
	MSBuild.exe /property:Configuration=Release /property:Platform="Any CPU" /target:%%t /verbosity:normal ".\MHash384_DotNet.sln"
	if not "!ERRORLEVEL!"=="0" (
		echo Error: Build has failed^^!
		pause
		goto:eof
	)
)

mkdir "%PACK_PATH%\dotnet"
copy /Y /B "libmhash384\bin\Release\mhash384*.dll" "%PACK_PATH%\dotnet"

popd

set CSHARP_VERSION=
for /F "tokens=*" %%a in ('csc.exe') do (
	if "!CSHARP_VERSION!"=="" (
		if not "%%~a"=="" set "CSHARP_VERSION=%%~a"
	)
)

echo MHash-384 v%MHASH384_VERSION_STR% for Microsoft.NET>        "%PACK_PATH%\dotnet\BUILD_TAG.txt"
echo Built on %ISO_DATE% at %ISO_TIME%, using %CSHARP_VERSION%>> "%PACK_PATH%\dotnet\BUILD_TAG.txt"

REM -----------------------------------------------------------
REM JAVA PORT
REM -----------------------------------------------------------

pushd "%~dp0\ports\java"

set "PATH=%JAVA_HOME%\bin;%ANT_HOME%\bin;%PATH%"

call ant
if not "%ERRORLEVEL%"=="0" (
	echo Error: Build has failed^^!
	pause
	goto:eof
)

mkdir "%PACK_PATH%\java"
copy /Y /B "out\mhash384*.jar" "%PACK_PATH%\java"

popd

set JAVA_VERSION=
for /F "tokens=*" %%a in ('java.exe -version 2^>^&1') do (
	if not "%%~a"=="" set (
		if "!JAVA_VERSION!"=="" set "JAVA_VERSION=%%~a"
	)
)

echo MHash-384 v%MHASH384_VERSION_STR% for Java>               "%PACK_PATH%\java\BUILD_TAG.txt"
echo Built on %ISO_DATE% at %ISO_TIME%, using %JAVA_VERSION%>> "%PACK_PATH%\java\BUILD_TAG.txt"

REM -----------------------------------------------------------
REM PYTHON PORT
REM -----------------------------------------------------------

pushd "%~dp0\ports\python"

set "PATH=%PYTHONHOME%;%PATH%"
set "PYTHONPATH=%CD%"

for %%f in (*.py) do (
	python.exe -c "import py_compile; py_compile.compile(\"%%~ff\", \"%%~dpnf.pyc\", optimize=1)"
)

mkdir "%PACK_PATH%\python"
copy /Y /B "mhash384*.pyc" "%PACK_PATH%\python"

popd

set PYTHON_VERSION=
for /F "tokens=*" %%a in ('python.exe -c "import sys; print(sys.implementation.name+\" \"+sys.version)"') do (
	if not "%%~a"=="" (
		if "!PYTHON_VERSION!"=="" set "PYTHON_VERSION=%%~a"
	)
)

echo MHash-384 v%MHASH384_VERSION_STR% for Python>               "%PACK_PATH%\python\BUILD_TAG.txt"
echo Built on %ISO_DATE% at %ISO_TIME%, using %PYTHON_VERSION%>> "%PACK_PATH%\python\BUILD_TAG.txt"

REM -----------------------------------------------------------
REM DELPHI PORT
REM -----------------------------------------------------------

pushd "%~dp0\ports\delphi"
set "PATH=%DELPHI_HOME%\bin;%PATH%"

pushd "test"
dcc32.exe -B -CC -$O+ "MHash384Test.dpr"
popd

mkdir "%PACK_PATH%\delphi"

copy /Y /B "test\*.exe"        "%PACK_PATH%\delphi"
copy /Y /B "libmhash384\*.dcu" "%PACK_PATH%\delphi"

popd

set DELPHI_VERSION=
for /F "tokens=*" %%a in ('dcc32.exe --version') do (
	if not "%%~a"=="" (
		if "!DELPHI_VERSION!"=="" set "DELPHI_VERSION=%%~a"
	)
)

echo MHash-384 v%MHASH384_VERSION_STR% for Delphi>               "%PACK_PATH%\delphi\BUILD_TAG.txt"
echo Built on %ISO_DATE% at %ISO_TIME%, using %DELPHI_VERSION%>> "%PACK_PATH%\delphi\BUILD_TAG.txt"

REM =========================================================================
REM CREATE DOCUMENTS
REM =========================================================================

set "PACKAGE_NAMES=win-x86,win-x64,java,dotnet,python,delphi"

"%PANDOC_PATH%\pandoc.exe" --from markdown_github+pandoc_title_block+header_attributes+implicit_figures --to html5 --toc -N --standalone -H "%~dp0\..\Prerequisites\Pandoc\css\github-pandoc.inc" -o %~dp0\README.html "%~dp0\README.md"
if not "%ERRORLEVEL%"=="0" (
	echo Error: Failed to generate documents^^!
	pause
	goto:eof
)

for %%p in (%PACKAGE_NAMES%) do (
	mkdir                                 "%PACK_PATH%\%%p\img"
	mkdir                                 "%PACK_PATH%\%%p\img\mhash384"
	copy /Y /B "%~dp0\README.html"        "%PACK_PATH%\%%p"
	copy /Y /B "%~dp0\COPYING.txt"        "%PACK_PATH%\%%p"
	copy /Y /B "%~dp0\img\mhash384\*.jpg" "%PACK_PATH%\%%p\img\mhash384"
	attrib +R                             "%PACK_PATH%\%%p\*.*" /S
)

REM =========================================================================
REM GENERATE OUTNAME
REM =========================================================================

if not exist "%~dp0\out\" (
	mkdir "%~dp0\out"
)

set "OUTNAME=MHash384.%ISO_DATE%"

:validate_outname
for %%p in (%PACKAGE_NAMES%) do (
	if exist "%~dp0\out\%OUTNAME%.%%p.zip" goto:next_outname
)
goto:outname_generated

:next_outname
set "OUTNAME=%OUTNAME%.new"
goto:validate_outname

:outname_generated

REM =========================================================================
REM CREATE DISTRIBUTION PACKAGE
REM =========================================================================

for %%p in (%PACKAGE_NAMES%) do (
	pushd "%PACK_PATH%\%%p"
	if not "!ERRORLEVEL!"=="0" (
		echo Error: Failed to change directory to PACK_PATH^^!
		pause
		goto:eof
	)
	"%~dp0\etc\utils\zip.exe" -r -9 -z "%~dp0\out\%OUTNAME%.%%p.zip" "*.*" < "%~dp0\COPYING.txt"
	if not "!ERRORLEVEL!"=="0" (
		echo Error: Failed to create ZIP package^^!
		pause
		goto:eof
	)
	popd
)

"%GIT_PATH%\bin\git.exe" archive --verbose --format=tar.gz --output "%~dp0\out\%OUTNAME%.source.tgz" HEAD
if not "%ERRORLEVEL%"=="0" (
	echo Error: Failed to export source codes^^!
	pause
	goto:eof
)

attrib +R "%~dp0\out\%OUTNAME%.*.zip"
attrib +R "%~dp0\out\%OUTNAME%.source.tgz"

REM =========================================================================
REM FINAL CLEAN-UP
REM =========================================================================

rmdir /Q /S "%PACK_PATH%"

echo.
echo COMPLETED
echo.

pause
