@echo off

REM Usage: btest.bat {target}|all [r]

REM Required Environment Variables in buildSpecs.bat.
REM testBuildDir - The path to the test generator build dir relative to
REM   buildSpecs.bat.
REM generator - The identifier used to call the generator from command line.

REM Required Arguments
REM {target} is the name of the target to build. all can be provided instead of
REM   the target name to build all possible targets.

REM Optional Arguments
REM r - After the target is built, the target executable will be executed.

if "%1" == "" (
  echo Error: A target name or all must be provided as the first argument.
  goto:eof
)

pushd "../"
call buildSpecs.bat

if "%testBuildDir%" == "" (
  echo Error: The environment variable "testBuildDir" must be set in ../buildSpecs.bat.
  popd
  goto:eof
)
if not exist %testBuildDir% (
  echo Error: The value of testBuildDir "%testBuildDir%" set in ../buildSpecs.bat does not exist relative to buildSpecs.bat.
  popd
  goto:eof
)

pushd %testBuildDir%
if "%1" == "all" (
  %generator%
) else (
  %generator% %1
)
popd
popd

set buildError=1
if %ERRORLEVEL% EQU %buildError% (
  goto:eof
)

if "%2" == "r" (
  echo =Running Target=
  %1.exe
)
