@echo off

REM Usage: btest.bat {target}|all [r [c|d]]

REM Required Environment Variables in buildSpecs.bat.
REM testBuildDir - The path to the test generator build dir relative to
REM   buildSpecs.bat.
REM generator - The identifier used to call the generator from command line.

REM Required Arguments
REM {target} is the name of the target to build. all can be provided instead of
REM   the target name to build all possible targets.

REM Optional Arguments
REM r - After the target is built, rtest.bat will be called with the built
REM   target as the argument. This and the following arguments will be ignored
REM   if all is provided instead of a specific target.
REM c|d - If one of these arguments is provided, it will be forwared to
REM   rtest.bat. Look in rtest.bat to read about their purpose.

if "%1" == "" (
  echo Error: A target name or all must be provided as the first argument.
  goto:eof
)

pushd "../"
call buildSpecs.bat

if "%testBuildDir%" == "" (
  echo Error: The environment variable "testBuildDir" must be set in
  echo ../buildSpecs.bat.
  popd
  goto:eof
)
if not exist %testBuildDir% (
  echo Error: The value of testBuildDir "%testBuildDir%" set in
  echo ../buildSpecs.bat does not exist relative to buildSpecs.bat.
  popd
  goto:eof
)

pushd %testBuildDir%
if "%1" == "all" (
  %generator%
  goto:eof
) else (
  %generator% %1
)
popd
popd

set buildError=1
if %ERRORLEVEL% EQU %buildError% (
  goto:eof
)

echo =Target Built=
if "%2" == "r" (
  rtest.bat %1 %3
)
