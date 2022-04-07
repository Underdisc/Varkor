@echo off

REM Usage: build target [r]
REM target - The target to build.
REM r - The target will run after a successful build.

REM Ensure the integrity of the build specifications and that there is a target.
call checkBuildSpecs.bat
set buildSpecsCheckFailed=1
if errorlevel %buildSpecsCheckFailed% (
  exit /b 1
)
if "%1" == "" (
  echo Error: The target to build must be specified as the first argument.
  exit /b 1
)

REM Build the target.
set target=%1
pushd %buildDir%
%generator% %target%
popd
set buildFailed=1
if errorlevel %buildFailed% (
  exit /b 1
)

REM If requested, run the target with the given arguments.
if "%2" == "r" (
  setlocal ENABLEDELAYEDEXPANSION
  set "args=%3"
  :NextArg
  if "%4" == "" goto AllArgsCollected
  set "args=!args! %4"
  shift
  goto NextArg
  :AllArgsCollected
  %target%.exe %args%
  endlocal
  exit /b 0
)
if not "%2" == "" (
  echo Error: %2 is not a valid second argument. Only r is valid.
  exit /b 1
)
exit /b 0
