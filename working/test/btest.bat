@echo off

REM Usage: btest.bat all|({target} [r|c|d])

REM Required Environment Variables to be set in buildSpecs.bat.
REM buildDir - The path to the build directory relative to buildSpecs.bat.
REM generator - The identifier used to call the generator.

REM Required Arguments
REM all - Build all of the existing test targets.
REM {target} - Build only the provided target.

REM Optional Arguments
REM r - Runs the target executable if successfully built.
REM c - Creates a file containing the output of the target executable named
REM   {target}_out.txt. The file will be overwritten if it already exists.
REM d - Creates a file containing the output of the target executable named
REM   {target}_out_diff.txt and diffs it against {target}_out.txt.

if "%1" == "" (
  echo Error: A target name or all must be provided as the first argument.
  goto:eof
)

REM Ensure that an existing build directory was selected.
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

REM Build the target.
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

REM The target has been built and we will now run the target if requested.
if "%2" == "r" (
  %1.exe
  goto:eof
)
if "%2" == "c" (
  %1.exe > %1_out.txt
  goto:eof
)
if "%2" == "d" (
  %1.exe > %1_out_diff.txt
  git diff --no-index %1_out.txt %1_out_diff.txt
  goto:eof
)
if not "%2" == "" (
  echo Error: %2 is not a valid argument. Only r, c, or d is valid.
)
