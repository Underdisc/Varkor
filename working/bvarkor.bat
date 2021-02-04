@echo off

REM Usage: bvarkor.bat [r]

REM Required Environment Variables in buildSpecs.bat.
REM buildDir - The path to the build dir relative to buildSpecs.bat.
REM generator - The identifier used to call the generator.

REM Optional Arguments
REM r - Varkor will run after a successful build.

REM Ensure that the build specifications exist.
call checkBuildSpecs.bat
set build_specs_check_failed=1
if %ERRORLEVEL% EQU %build_specs_check_failed% (
  goto:eof
)

REM Build Varkor.
pushd %buildDir%
%generator% varkor
popd
set build_failed=1
if %ERRORLEVEL% EQU %build_failed% (
  goto:eof
)

REM Run Varkor if requested.
if "%1" == "r" (
  varkor.exe
)
