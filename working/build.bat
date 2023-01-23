@echo off
REM Usage: build compiler configuration target [r] [args]
REM compiler - The compiler to build with.
REM configuration - The type of build (dbg, rel, relDbg, relMin).
REM target - The target to build.
REM r - The target will run after a successful build.
REM args - Arguments given when invoking the built executable.

set scriptDir=%~dp0
set compiler=%1
set configuration=%2
set target=%3
set action=%4

rem Collect arguments for when we run the built target.
set args=
:NextArg
if "%5" == "" goto AllArgsCollected
set args=%args% %5
shift
goto NextArg
:AllArgsCollected

REM Ensure that build specifications are set.
call checkBuildSpecs.bat
if errorlevel 1 (
  exit /b 1
)
set buildDir=%scriptDir%\..\build\%compiler%\%configuration%

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
