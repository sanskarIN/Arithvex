@echo off
setlocal
set "REPO_ROOT=%~dp0.."
cmake -S "%REPO_ROOT%" -B "%REPO_ROOT%\build" -DBUILD_TESTING=ON
if errorlevel 1 exit /b %errorlevel%
cmake --build "%REPO_ROOT%\build" --config Release --parallel
if errorlevel 1 exit /b %errorlevel%
ctest --test-dir "%REPO_ROOT%\build" -C Release --output-on-failure
exit /b %errorlevel%
