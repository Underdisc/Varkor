@echo off

REM Usage: btest.bat (all [t])|({target} [r|c|d|t])

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
REM t - Does the same as the d option, but only prints out "{target}: Passed"
REM   or "{target}: Failed" depending on the result of the diff. When this
REM   option is used with the all argument, every existing test will be executed
REM   and dispalyed with "{target}: Passed" or {target}: Failed".

REM Ensure that the build specifications exist.
pushd "../"
call checkBuildSpecs.bat
set buildSpecsCheckFailed=1
if errorlevel %buildSpecsCheckFailed% (
  popd
  goto:eof
)

REM Build the target or targets.
if "%1" == "" (
  echo Error: all or a target name must be provided as the first argument.
  popd
  goto:eof
)
pushd %buildDir%
if "%1" == "all" (
  %generator% tests
) else (
  %generator% %1
)
popd
popd
set buildError=1
if errorlevel %buildError% (
  goto:eof
)

REM Run all of the test targets if requested.
if "%1" == "all" (
  if "%2" == "t" (
    for /f %%t in (testNames.txt) do call :PerformTest %%t
    goto:eof
  )
  if not "%2" == "" (
    echo Error: %2 is not a valid argument. Only t is valid.
  )
  goto:eof
)

REM Run a single target with the desired output if requested.
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
if "%2" == "t" (
  call :PerformTest %1
  goto:eof
)
if not "%2" == "" (
  echo Error: %2 is not a valid argument. Only r, c, d, or t are valid.
)
goto:eof

:PerformTest
  %1.exe > %1_out_diff.txt
  git diff --no-index --no-patch --exit-code %1_out.txt %1_out_diff.txt
  if errorlevel 1 (
    echo %1: [31mFailed[0m
  ) else (
    echo %1: [32mPassed[0m
  )
exit /b 0
