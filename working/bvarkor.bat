@echo off

REM Usage: bvarkor.bat [r]

REM Required Environment Variables in buildSpecs.bat.
REM buildDir - The path to the build dir relative to buildSpecs.bat.
REM generator - The identifier used to call the generator.

REM Optional Arguments
REM r - Varkor will run after a successful build.

call buildSpecs.bat

if "%buildDir%" == "" (
  echo Error: The environment variable "buildDir" must be set in
  echo buildSpecs.bat.
  goto:eof
)
if not exist %buildDir% (
  echo Error: The value of buildDir [%buildDir%] does not exist relative to
  echo buildSpecs.bat.
  goto:eof
)

pushd %buildDir%
%generator% varkor
popd

set build_failed=1
if %ERRORLEVEL% EQU %build_failed% (
  goto:eof
)

if "%1" == "r" (
  echo =Running=
  varkor.exe
)
