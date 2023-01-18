@echo off
rem Usage: initializeBuildFiles compilerDir
rem Creates or recreates all of the build files needed for different build
rem   types for a specific compiler.

rem Required Arguments
rem compilerDir - The directory within root/build where the build type
rem   directories will be created.

set compilerDir=%1
if "%compilerDir%" == "" (
  echo Error: compilerDir argument missing.
  exit /b 0
)

pushd "../"
if not exist build/ mkdir build
pushd build

if exist %compilerDir%/ rm -rf %compilerDir%
mkdir %compilerDir%
pushd %compilerDir%

call :InitializeBuildDir dbg, Debug
call :InitializeBuildDir rel, Release
call :InitializeBuildDir relDbg, RelWithDebInfo
call :InitializeBuildDir relMin, MinSizeRel

popd
popd
popd
exit /b 0

:InitializeBuildDir
  echo Creating %~2 (%~1) build files.
  mkdir %~1
  pushd %~1
  cmake -G "Ninja" -DCMAKE_BUILD_TYPE=%~2 ../../../
  popd
exit /b 0

