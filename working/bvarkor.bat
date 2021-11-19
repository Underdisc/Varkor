@echo off

REM Usage: bvarkor.bat [r]

REM Required Environment Variables in buildSpecs.bat.
REM buildDir - The path to the build dir relative to buildSpecs.bat.
REM generator - The identifier used to call the generator.

REM Optional Arguments
REM r - Varkor will run after a successful build.

REM Ensure that the build specifications exist.
call checkBuildSpecs.bat
set buildSpecsCheckFailed=1
if errorlevel %buildSpecsCheckFailed% (
  exit /b 1
)

REM Build Varkor.
pushd %buildDir%
%generator% varkor
popd
set buildFailed=1
if errorlevel %buildFailed% (
  exit /b 1
)

REM If requested, run Varkor with the remaining arguments.
if "%1" == "r" (
  setlocal ENABLEDELAYEDEXPANSION
  set "varkorArgs=%2"
  :NextArg
  if "%3" == "" goto AllArgsCollected
  set "varkorArgs=!varkorArgs! %3"
  shift
  goto NextArg
  :AllArgsCollected
  varkor.exe %varkorArgs%
  endlocal
  exit /b 0
)
if not "%1" == "" (
  echo Error: %1 is not a valid argument. Only r is valid.
  exit /b 1
)
exit /b 0
