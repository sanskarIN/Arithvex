@echo off
setlocal
call "%~dp0build-windows.cmd"
if errorlevel 1 exit /b %errorlevel%
"%~dp0..\build\Release\ArithvexCLI.exe"
exit /b %errorlevel%
