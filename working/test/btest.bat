@echo off
REM Usage: btest buildType (all [t|c]) | ({target} [r|d|a|t|c])

REM Required Arguments
REM all - Build all of the existing test targets.
REM {target} - Build only the provided target.

REM Optional Arguments
REM r - Runs the target executable if successfully built.
REM d - Creates a file containing the output of the target executable named
REM   {target}/out_diff.txt and git diffs it against {target}/out.txt.
REM a - Creates a file containing the output of the target executable named
REM   {target}/out.txt. The file will be overwritten if it already exists.
REM t - Does the same as the d option, but only prints out "{target}: Passed"
REM   or "{target}: Failed" depending on the result of the diff. When this
REM   option is used with the all argument, every existing test will be executed
REM   and dispalyed with "{target}: Passed" or {target}: Failed".
REM c - Creates a directory named {target}/coverage that contains a code
REM   coverage report. This makes finding the code that a unit test did and did
REM   not run easy. A build of OpenCppCoverage needs to be in your path. It can
REM   be found here (https://github.com/OpenCppCoverage/OpenCppCoverage).

set buildType=%1
set target=%2

REM Ensure that the build specifications are set.
pushd "../"
call checkBuildSpecs.bat
if errorlevel 1 (
  popd
  exit /b 1
)

REM Build the target or targets.
pushd "../build/%compilerDir%/%buildType%"
if "%target%" == "all" (
  ninja tests
) else (
  ninja %target%
)
popd
popd
if errorlevel 1 (
  exit /b 1
)

REM Run all of the test targets if requested.
if "%target%" == "all" (
  if "%3" == "t" (
    for /d %%d in (*) do call :PerformTest %%d
    exit /b 0
  )
  if "%3" == "c" (
    for /d %%d in (*) do call :GenerateCoverage %%d
    exit /b 0
  )
  if not "%3" == "" (
    echo Error: %3 is not a valid argument. Look inside btest.bat for details.
    exit /b 1
  )
  exit /b 0
)

REM Run a single target with the desired output if requested.
if "%3" == "r" (
  pushd %target%
  test_%buildType%.exe
  popd
  exit /b 0
)
if "%3" == "d" (
  pushd %target%
  test_%buildType%.exe > out_diff.txt
  git -c core.fileMode=false diff --no-index --unified=10 out.txt out_diff.txt
  popd
  exit /b 0
)
if "%3" == "a" (
  pushd %target%
  test_%buildType%.exe > out.txt
  popd
  exit /b 0
)
if "%3" == "t" (
  call :PerformTest %target%
  exit /b 0
)
if "%3" == "c" (
  call :GenerateCoverage %target%
  exit /b 0
)
if not "%3" == "" (
  echo Error: %3 is not a valid argument. Look inside btest.bat for details.
  exit /b 1
)
exit /b 0

:PerformTest
  pushd %~1
  test_%buildType%.exe > out_diff.txt
  git -c core.fileMode=false diff --no-index --no-patch --exit-code out.txt out_diff.txt
  if errorlevel 1 (
    echo %~1: [31mFailed[0m
  ) else (
    echo %~1: [32mPassed[0m
  )
  popd
exit /b 0

:GenerateCoverage
  setlocal ENABLEDELAYEDEXPANSION
  pushd "../../"
  OpenCppCoverage -q ^
    --sources !cd!\src\* ^
    --working_dir working\test\%~1 ^
    --export_type html:working\test\%~1\coverage ^
    -- working\test\%~1\test_%buildType%.exe > LastCoverageResults.log
  del LastCoverageResults.log
  popd
  endlocal
exit /b 0
