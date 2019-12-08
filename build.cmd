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
	echo MSVC not found. Please check MSVC_PATH and try again^^!
	pause
	goto:eof
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

REM =========================================================================
REM SET UP ENVIRONMENT
REM =========================================================================

set "PATH=%GIT_PATH%\bin;%ANT_HOME%\bin;%JAVA_HOME%\bin;%PATH%"

set VCINSTALLDIR=
call "%MSVC_PATH%\vcvarsall.bat" x86

if "%VCINSTALLDIR%"=="" (
	echo Error: Failed to set up MSVC environment^^!
	pause
	goto:eof
)

REM =========================================================================
REM DETECT BUILD DATE
REM =========================================================================

set "ISO_DATE="

for /F "tokens=1,2 delims=:" %%a in ('"%~dp0\etc\utils\date.exe" +ISODATE:%%Y-%%m-%%d') do (
	if "%%a"=="ISODATE" set "ISO_DATE=%%b"
)

if "%ISO_DATE%"=="" (
	echo Error: Failed to determine build date^^!
	pause
	goto:eof
)

REM =========================================================================
REM REMOVE EXISTING BINARIES
REM =========================================================================

if exist "%~dp0\bin\" (
	rmdir /S /Q "%~dp0\bin"
)

if exist "%~dp0\obj\" (
	rmdir /S /Q "%~dp0\obj"
)

REM =========================================================================
REM BUILD BINARIES
REM =========================================================================

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

REM -----------------------------------------------------------
REM C# PORT
REM -----------------------------------------------------------

pushd "%~dp0\ports\c-sharp"

for %%t in (Clean,Rebuild) do (
	MSBuild.exe /property:Configuration=Release /property:Platform="Any CPU" /target:%%t /verbosity:normal ".\MHash384_DotNet.sln"
	if not "!ERRORLEVEL!"=="0" (
		echo Error: Build has failed^^!
		pause
		goto:eof
	)
)

popd

REM -----------------------------------------------------------
REM JAVA PORT
REM -----------------------------------------------------------

pushd "%~dp0\ports\java"

call ant
if not "%ERRORLEVEL%"=="0" (
	echo Error: Build has failed^^!
	pause
	goto:eof
)

popd

REM =========================================================================
REM COPY DISTRIBUTION FILES
REM =========================================================================

echo.
echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
echo CREATE PACKAGE
echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
echo.

set "PACK_PATH=%TMP%\~%RANDOM%$%RANDOM%$%RANDOM%.tmp"

mkdir "%PACK_PATH%"
for %%p in (win-x86,win-x64,java,dotnet) do (
	mkdir "%PACK_PATH%\%%p"
)

copy /Y "%~dp0\bin\Win32\Release\MHash384.exe"                      "%PACK_PATH%\win-x86"
copy /Y "%~dp0\bin\x64\Release\MHash384.exe"                        "%PACK_PATH%\win-x64"
copy /Y "%~dp0\ports\java\out\mhash384*.jar"                        "%PACK_PATH%\java"
copy /Y "%~dp0\ports\c-sharp\libmhash384\bin\Release\mhash384*.dll" "%PACK_PATH%\dotnet"

REM =========================================================================
REM CREATE DOCUMENTS
REM =========================================================================

"%PANDOC_PATH%\pandoc.exe" --from markdown_github+pandoc_title_block+header_attributes+implicit_figures --to html5 --toc -N --standalone -H "%~dp0\..\Prerequisites\Pandoc\css\github-pandoc.inc" -o %~dp0\README.html "%~dp0\README.md"
if not "%ERRORLEVEL%"=="0" (
	echo Error: Failed to generate documents^^!
	pause
	goto:eof
)

for %%p in (win-x86,win-x64,java,dotnet) do (
	mkdir                              "%PACK_PATH%\%%p\img"
	mkdir                              "%PACK_PATH%\%%p\img\mhash384"
	copy /Y "%~dp0\README.html"        "%PACK_PATH%\%%p"
	copy /Y "%~dp0\COPYING.txt"        "%PACK_PATH%\%%p"
	copy /Y "%~dp0\img\mhash384\*.jpg" "%PACK_PATH%\%%p\img\mhash384"
	attrib +R "%PACK_PATH%\%%p\*.*" /S
)

for %%p in (win-x86,win-x64) do (
	"%PACK_PATH%\%%p\MHash384.exe" --version > "%PACK_PATH%\%%p\BUILD_TAG.txt"
)

REM =========================================================================
REM GENERATE OUTNAME
REM =========================================================================

if not exist "%~dp0\out\" (
	mkdir "%~dp0\out"
)

set "OUTNAME=MHash384.%ISO_DATE%"

:validate_outname
for %%p in (win-x86,win-x64,java,dotnet) do (
	if exist "%~dp0\out\%OUTNAME%.%%p.zip" goto:next_outname
)
goto:outname_generated

:next_outname
set "OUTNAME=%OUTNAME%.new"
goto:validate_outname

:outname_generated

REM =========================================================================
REM CREATE PACKAGE
REM =========================================================================

for %%p in (win-x86,win-x64,java,dotnet) do (
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

REM =========================================================================
REM COMPLETED
REM =========================================================================

echo.
echo COMPLETED
echo.

pause
