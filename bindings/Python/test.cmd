@echo off
set "PYTHONHOME=C:\Program Files\Python36"
set "PYTHONPATH=%~dp0\library"
"%PYTHONHOME%\python.exe" "%~dp0\test.py"
pause
