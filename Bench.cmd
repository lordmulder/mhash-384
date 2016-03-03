@echo off
"%~dp0\tools\DevRandom\bin\Win32\Release\DevRandom.exe" 31457280 1553932631 > "%TMP%\test.rnd"
"%~dp0\tools\timedexec.exe" "%~dp0\bin\Win32\Release\mhash_384.x86.exe" %TMP%\test.rnd
del "%TMP%\test.rnd"
pause
