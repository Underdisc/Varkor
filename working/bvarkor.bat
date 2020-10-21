@echo off

REM Usage: bvarkor.bat [r]

REM Required Environment Variables in buildSpecs.bat
REM varkorBuildDir - The path to the directory that the generator command will
REM   run from to build Varkor.
REM generator - The command used to call the generator from command line.

REM Optional Arguments
REM r - Varkor will run after a successful build.

call buildSpecs.bat

if "%varkorBuildDir%" == "" (
  echo Error: The environment variable "varkorBuildDir" must be set in
  echo buildSpecs.bat.
  goto:eof
)
if not exist %varkorBuildDir% (
  echo Error: The value of varkorBuildDir "%varkorBuildDir%" set in
  echo buildSpecs.bat does not exist relative to buildSpecs.bat.
  goto:eof
)

pushd %varkorBuildDir%
%generator%
popd

set build_failed=1
if %ERRORLEVEL% EQU %build_failed% (
  goto:eof
)

if "%1" == "r" (
  echo =Running=
  varkor.exe
)
