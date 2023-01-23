@echo off
rem Usage: initializeBuildDirs
rem Creates all of the build directories for supported Windows compilers.

set scriptDir=%~dp0

pushd "%scriptDir%\..\"
if not exist build\ mkdir build
popd
set buildDir=%scriptDir%\..\build

call :InitializeCompilerBuildDirs msvc64
call :InitializeCompilerBuildDirs clang64
exit /b 0

:InitializeCompilerBuildDirs
  pushd "%buildDir%"
  if not exist %~1 mkdir %~1
  pushd %~1
  if not exist dbg\ mkdir dbg
  if not exist rel\ mkdir rel
  if not exist relDbg\ mkdir relDbg
  if not exist relMin\ mkdir relMin
  popd
  popd
exit /b 0

