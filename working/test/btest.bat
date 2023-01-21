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

set scriptDir=%~dp0
set buildType=%1
set target=%2
set action=%3

set rootDir=%scriptDir%/../..
set buildDir=%rootDir%/build/%compilerDir%/%buildType%

REM Ensure that the build specifications are set.
pushd "%scriptDir%/../"
call checkBuildSpecs.bat
if errorlevel 1 (
  popd
  exit /b 1
)
popd

REM Build the target or targets.
pushd "%buildDir%"
if "%target%" == "all" (
  ninja tests
) else (
  ninja %target%
)
popd
if errorlevel 1 (
  exit /b 1
)

REM Run all of the test targets if requested.
if "%target%" == "all" (
  if "%action%" == "t" (
    for /d %%d in (*) do call :PerformTest %%d
    exit /b 0
  )
  if "%action%" == "c" (
    for /d %%d in (*) do call :GenerateCoverage %%d
    exit /b 0
  )
  if not "%action%" == "" (
    echo Error: %action% is not a valid action. Look inside btest.bat for details.
    exit /b 1
  )
  exit /b 0
)

REM Run a single target with the desired output if requested.
if "%action%" == "r" (
  call :RunTest %target%
  exit /b 0
)
if "%action%" == "d" (
  call :DiffTest %target%
  exit /b 0
)
if "%action%" == "a" (
  call :OverwriteTest %target%
  exit /b 0
)
if "%action%" == "t" (
  call :PerformTest %target%
  exit /b 0
)
if "%action%" == "c" (
  call :GenerateCoverage %target%
  exit /b 0
)
if not "%action%" == "" (
  echo Error: %action% is not a valid argument. Look inside btest.bat for details.
  exit /b 1
)
exit /b 0

:AcquireTarget
  pushd "%buildDir%/src/test"
  move %~1.* %scriptDir%/%~1 > nul
  popd
  pushd "%scriptDir%/%~1"
exit /b 0

:ReturnTarget
  move %~1.* %buildDir%/src/test > nul
  popd
exit /b 0

:RunTest
  call :AcquireTarget %~1
  %~1.exe
  call :ReturnTarget %~1
exit /b 0

:DiffTest
  call :AcquireTarget %~1
  %~1.exe > out_diff.txt
  git -c core.fileMode=false diff --no-index --unified=10 out.txt out_diff.txt
  call :ReturnTarget %~1
exit /b 0

:OverwriteTest
  call :AcquireTarget %~1
  %1.exe > out.txt
  call :ReturnTarget %~1
exit /b 0

:PerformTest
  call :AcquireTarget %~1
  %~1.exe > out_diff.txt
  git -c core.fileMode=false diff --no-index --no-patch --exit-code out.txt out_diff.txt
  if errorlevel 1 (
    echo %~1: [31mFailed[0m
  ) else (
    echo %~1: [32mPassed[0m
  )
  call :ReturnTarget %~1
exit /b 0

:GenerateCoverage
  call :AcquireTarget %~1
  pushd "%rootDir%"
  setlocal ENABLEDELAYEDEXPANSION
  OpenCppCoverage -q ^
    --sources src\* ^
    --working_dir working\test\%~1 ^
    --export_type html:working\test\%~1\coverage ^
    -- working\test\%~1\%~1.exe > nul
  endlocal
  popd
  call :ReturnTarget %~1
exit /b 0