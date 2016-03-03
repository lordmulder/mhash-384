@echo off
setlocal enabledelayedexpansion
cd /d "%~dp0"

REM ///////////////////////////////////////////////////////////////////////////
REM // Setup environment
REM ///////////////////////////////////////////////////////////////////////////

set "MSVC_PATH=C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC"
set "PDOC_PATH=C:\Program Files (x86)\Pandoc"
set "GIT2_PATH=C:\Program Files\Git\bin"


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

"%~dp0\tools\cecho" YELLOW "\n========[ CLEAN UP ]========\n"

for %%i in (bin,obj) do (
	del /Q /S /F "%~dp0\%%i\*.*"
)


REM ///////////////////////////////////////////////////////////////////////////
REM // Build the binaries
REM ///////////////////////////////////////////////////////////////////////////

"%~dp0\tools\cecho" YELLOW "\n========[ COMPILE ]========\n"
call "%MSVC_PATH%\vcvarsall.bat"

for %%p in (x86,x64) do (
	MSBuild.exe /property:Platform=%%p /property:Configuration=Release /target:Clean   "%~dp0\MHashLib.sln"
	if not "!ERRORLEVEL!"=="0" goto BuildHasFailed
	MSBuild.exe /property:Platform=%%p /property:Configuration=Release /target:Rebuild "%~dp0\MHashLib.sln"
	if not "!ERRORLEVEL!"=="0" goto BuildHasFailed
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
set "OUT_PATH_X86=%~dp0\out\mhash_384.%ISO_DATE%%REVISON%.msvc-x86.zip"
set "OUT_PATH_X64=%~dp0\out\mhash_384.%ISO_DATE%%REVISON%.msvc-x64.zip"
set "OUT_PATH_SRC=%~dp0\out\mhash_384.%ISO_DATE%%REVISON%.src.tar.gz"

set /a COUNTER=COUNTER+1
set REVISON=.update-%COUNTER%

if exist "%OUT_PATH_X86%" goto GenerateOutfileNameNext
if exist "%OUT_PATH_X64%" goto GenerateOutfileNameNext
if exist "%OUT_PATH_SRC%" goto GenerateOutfileNameNext


REM ///////////////////////////////////////////////////////////////////////////
REM // Build ZIP package
REM ///////////////////////////////////////////////////////////////////////////

"%~dp0\tools\cecho" YELLOW "\n========[ PACKAGING ]========\n"

"%~dp0\tools\zip.exe" -j -9 -z "%OUT_PATH_X86%" "%~dp0\bin\Win32\Release\mhash_384.x86.exe" "%~dp0\README.html" "%~dp0\COPYING.txt" < "%~dp0\COPYING.txt"
"%~dp0\tools\zip.exe" -j -9 -z "%OUT_PATH_X64%" "%~dp0\bin\x64\.\Release\mhash_384.x64.exe" "%~dp0\README.html" "%~dp0\COPYING.txt" < "%~dp0\COPYING.txt"

"%GIT2_PATH%\git.exe" archive --format tar.gz -9 --verbose --output "%OUT_PATH_SRC%" HEAD


REM ///////////////////////////////////////////////////////////////////////////
REM // Completed
REM ///////////////////////////////////////////////////////////////////////////

"%~dp0\tools\cecho" GREEN "\nBUILD COMPLETED.\n"
pause
goto:eof


REM ///////////////////////////////////////////////////////////////////////////
REM // Failed
REM ///////////////////////////////////////////////////////////////////////////

:BuildHasFailed
"%~dp0\tools\cecho" RED "\nBUILD HAS FAILED.\n"
pause
goto:eof