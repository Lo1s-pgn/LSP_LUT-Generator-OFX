@echo off
REM LSP Simple LUT Generator — Windows build (Ninja + MSVC).
REM Output: release\LSP_Simple_LUT_Generator_<version>_windows\LSP_Simple_LUT_Generator_<version>.ofx.bundle
REM Usage: tools\windows\lutgen_build.bat
REM        tools\windows\lutgen_build.bat clean

setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
if errorlevel 1 (
  call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
  if errorlevel 1 exit /b 1
)

set "PATH=C:\Users\Admin\AppData\Local\Microsoft\WinGet\Packages\Ninja-build.Ninja_Microsoft.Winget.Source_8wekyb3d8bbwe;%PATH%"

cd /d "%~dp0..\..\"

if not exist "build\windows\CMakeCache.txt" (
  cmake -S . -B build/windows -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl
  if errorlevel 1 exit /b 1
) else (
  cmake -S . -B build/windows -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl
  if errorlevel 1 exit /b 1
)

if /i "%~1"=="clean" (
  cmake --build build/windows --target clean
  if errorlevel 1 exit /b 1
)

cmake --build build/windows --target lutgen_all --parallel
exit /b %ERRORLEVEL%
