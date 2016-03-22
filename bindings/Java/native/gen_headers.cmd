@echo off

if not exist "%JDK_HOME%\bin\javah.exe" (
	echo javah.exe not found!
	pause & goto:eof
)

cd /d "%~dp0"
mkdir "include" 2> NUL

echo Generating JNI header file...
"%JDK_HOME%\bin\javah.exe" -force -o "include\mhash_384.jni.h" -cp "..\wrapper\src" mhash.MHash384

if not "%ERRORLEVEL%"=="0" (
	echo ERROR: Something went wrong!
	pause & goto:eof
)

echo Completed.
pause
