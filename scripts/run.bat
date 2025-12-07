@echo off
setlocal ENABLEEXTENSIONS ENABLEDELAYEDEXECUTION

REM Usage: scripts\run.bat [trace]
REM Run from project root: scripts\run.bat

set TRACEFLAG=
if /I "%1"=="trace" set TRACEFLAG=-DTRACE

set TARGET=ll_isort.exe
set SRCDIR=%~dp0..\src
set INCDIR=%~dp0..\include

where g++ >nul 2>nul
if %ERRORLEVEL%==0 (
  echo Building with g++ %TRACEFLAG%
  g++ -std=c++20 -O2 -Wall -Wextra -pedantic -I"%INCDIR%" %TRACEFLAG% "%SRCDIR%\main.cpp" -o %TARGET% || goto :err
  echo Running %TARGET%
  .\%TARGET%
  goto :eof
)

where clang++ >nul 2>nul
if %ERRORLEVEL%==0 (
  echo Building with clang++ %TRACEFLAG%
  clang++ -std=c++20 -O2 -Wall -Wextra -pedantic -I"%INCDIR%" %TRACEFLAG% "%SRCDIR%\main.cpp" -o %TARGET% || goto :err
  echo Running %TARGET%
  .\%TARGET%
  goto :eof
)

echo No C++ compiler found (g++ or clang++). Install MSYS2/MinGW, LLVM, or Visual Studio.
goto :eof

:err
echo Build failed.
exit /b 1