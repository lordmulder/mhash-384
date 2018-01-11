@echo off
set "PYPY3_HOME=D:\PyPy3"
set "PYTHONPATH=%~dp0\library"
"%PYPY3_HOME%\pypy3.exe" "%~dp0\start.py"
pause
