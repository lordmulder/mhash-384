@echo off
set "DRMEM_PATH=C:\Program Files (x86)\Dr. Memory\bin"
set "PATH=%DRMEM_PATH%;%PATH%"
pushd "%~dp0\bin\Win32\Release_DrMem"
"%DRMEM_PATH%\drmemory.exe" -- mhash_384.dr-mem.exe -t
