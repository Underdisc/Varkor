@echo off

REM Usage: build [r]
REM r - The target will run after a successful build.

REM Ensure that the build specifications exist.
call checkBuildSpecs.bat
set buildSpecsCheckFailed=1
if errorlevel %buildSpecsCheckFailed% (
  exit /b 1
)

REM Build the target.
pushd ..\%buildDir%
%generator% %target%
popd
set buildFailed=1
if errorlevel %buildFailed% (
  exit /b 1
)

REM If requested, run the target with the given arguments.
if "%1" == "r" (
  setlocal ENABLEDELAYEDEXPANSION
  set "args=%2"
  :NextArg
  if "%3" == "" goto AllArgsCollected
  set "args=!args! %3"
  shift
  goto NextArg
  :AllArgsCollected
  %target%.exe %args%
  endlocal
  exit /b 0
)
if not "%1" == "" (
  echo Error: %1 is not a valid argument. Only r is valid.
  exit /b 1
)
exit /b 0
