@echo off

REM Usage: btest.bat {target}|all [r [c|d]]

REM Required Environment Variables in buildSpecs.bat.
REM buildDir - The path to the build dir relative to buildSpecs.bat.
REM generator - The identifier used to call the generator.

REM Required Arguments
REM {target} - The name of the target to build. all can be provided instead of
REM   the target name to build all the test targets.

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

if "%buildDir%" == "" (
  echo Error: The environment variable "buildDir" must be set in
  echo ../buildSpecs.bat.
  popd
  goto:eof
)
if not exist %buildDir% (
  echo Error: The value of buildDir [%buildDir%] does not exist relative to
  echo ../buildSpecs.bat.
  popd
  goto:eof
)

pushd %buildDir%
if "%1" == "all" (
  %generator% tests
  popd
  popd
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
if not "%2" == "" (
  echo Error: "%2" is not a valid argument. Only "r" followed by nothing, c or
  echo d is valid.
)
