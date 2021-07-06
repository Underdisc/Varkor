@echo off

REM Usage: btest.bat (all [t|c])|({target} [r|d|a|t|c])

REM Required Environment Variables to be set in buildSpecs.bat.
REM buildDir - The path to the build directory relative to buildSpecs.bat.
REM generator - The identifier used to call the generator.

REM Required Arguments
REM all - Build all of the existing test targets.
REM {target} - Build only the provided target.

REM Optional Arguments
REM r - Runs the target executable if successfully built.
REM d - Creates a file containing the output of the target executable named
REM   {target}_out_diff.txt and git diffs it against {target}_out.txt.
REM a - Creates a file containing the output of the target executable named
REM   {target}_out.txt. The file will be overwritten if it already exists.
REM t - Does the same as the d option, but only prints out "{target}: Passed"
REM   or "{target}: Failed" depending on the result of the diff. When this
REM   option is used with the all argument, every existing test will be executed
REM   and dispalyed with "{target}: Passed" or {target}: Failed".
REM c - Creates a directory named {target}_coverage that contains a code
REM   coverage report. This makes finding the code that a unit test did and did
REM   not run easy. A build of OpenCppCoverage needs to be in your path. It can
REM   be found here (https://github.com/OpenCppCoverage/OpenCppCoverage).

REM Ensure that the build specifications exist.
pushd "../"
call checkBuildSpecs.bat
set buildSpecsCheckFailed=1
if errorlevel %buildSpecsCheckFailed% (
  popd
  exit /b 1
)

REM Build the target or targets.
if "%1" == "" (
  echo Error: all or a target name must be provided as the first argument.
  popd
  exit /b 1
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
  exit /b 1
)

REM Run all of the test targets if requested.
if "%1" == "all" (
  if "%2" == "t" (
    for /d %%d in (*) do call :PerformTest %%d
    exit /b 0
  )
  if "%2" == "c" (
    for /d %%d in (*) do call :GenerateCoverage %%d
    exit /b 0
  )
  if not "%2" == "" (
    echo Error: %2 is not a valid argument. Look inside btest.bat for details.
    exit /b 1
  )
  exit /b 0
)

REM Run a single target with the desired output if requested.
if "%2" == "r" (
  pushd %1
  test.exe
  popd
  exit /b 0
)
if "%2" == "d" (
  pushd %1
  test.exe > out_diff.txt
  git diff --no-index out.txt out_diff.txt
  popd
  exit /b 0
)
if "%2" == "a" (
  pushd %1
  test.exe > out.txt
  popd
  exit /b 0
)
if "%2" == "t" (
  call :PerformTest %1
  exit /b 0
)
if "%2" == "c" (
  call :GenerateCoverage %1
  exit /b 0
)
if not "%2" == "" (
  echo Error: %2 is not a valid argument. Look inside btest.bat for details.
  exit /b 1
)
exit /b 0

:PerformTest
  pushd %1
  test.exe > out_diff.txt
  git diff --no-index --no-patch --exit-code out.txt out_diff.txt
  if errorlevel 1 (
    echo %1: [31mFailed[0m
  ) else (
    echo %1: [32mPassed[0m
  )
  popd
exit /b 0

:GenerateCoverage
  setlocal ENABLEDELAYEDEXPANSION
  pushd "../../"
  OpenCppCoverage ^
    --sources !cd!\src\* --export_type html:working\test\%1\coverage -q -- ^
    working\test\%1\test.exe > LastCoverageResults.log
  del LastCoverageResults.log
  popd
  endlocal
exit /b 0
