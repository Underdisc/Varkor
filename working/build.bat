@echo off
REM Usage: build buildType target [r] [args]
REM buildType - The type of build (dbg, rel, relDbg, relMin).
REM target - The target to build.
REM r - The target will run after a successful build.
REM args - Arguments given when invoking the built executable.

set scriptDir=%~dp0
set buildType=%1
set target=%2
set action=%3

rem Collect arguments for when we run the built target.
set args=
:NextArg
if "%4" == "" goto AllArgsCollected
set args=%args% %4
shift
goto NextArg
:AllArgsCollected

set buildDir=%scriptDir%/../build/%compilerDir%/%buildType%

REM Ensure that build specifications are set.
call checkBuildSpecs.bat
if errorlevel 1 (
  exit /b 1
)

REM Build the target.
pushd "%buildDir%"
ninja %target%
popd
if errorlevel 1 (
  exit /b 1
)

REM If requested, run the target with the given arguments.
if "%action%" == "r" (
  pushd "%buildDir%"
  move %target%.* %scriptDir% > nul
  popd
  pushd "%scriptDir%"
  %target%.exe %args%
  move %target%.* %buildDir% > nul
  popd
) else (
  if not "%action%" == "" (
    echo Error: %action% is not a valid action. Only r is valid.
    exit /b 1
  )
)
exit /b 0
