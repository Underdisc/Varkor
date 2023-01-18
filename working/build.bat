@echo off

REM Usage: build buildType target [r] [args]
REM buildType - The type of build (dbg, rel, relDbg, relMin).
REM target - The target to build.
REM r - The target will run after a successful build.
REM args - Arguments given when invoking the built executable.

set buildType=%1
set target=%2

REM Ensure that build specifications are set.
call checkBuildSpecs.bat
if errorlevel 1 (
  exit /b 1
)

REM Build the target.
pushd "../build/%compilerDir%/%buildType%"
ninja %target%
popd
if errorlevel 1 (
  exit /b 1
)

REM If requested, run the target with the given arguments.
if "%3" == "r" (
  setlocal ENABLEDELAYEDEXPANSION
  set "args=%3"
  :NextArg
  if "%4" == "" goto AllArgsCollected
  set "args=!args! %4"
  shift
  goto NextArg
  :AllArgsCollected
  %target%_%buildType%.exe %args%
  endlocal
  exit /b 0
)
if not "%3" == "" (
  echo Error: %3 is not a valid argument. Only r is valid.
  exit /b 1
)
exit /b 0
