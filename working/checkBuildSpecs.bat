REM Ensures that all environment variables needed for build scripts are set.
REM compilerDir - The compiler directory within root/build/.
REM buildType - The type of build to be created.
REM target - The target to build.

call buildSpecs.bat
if "%compilerDir%" == "" (
  echo Error: buildSpecs.bat must set the "compilerDir" environment variable.
  exit /b 1
)

REM Ensure that a valid build type was given.
if "%buildType%" == "" (
  echo Error: A build type must be specified.
  exit /b 1
)
set validBuildType=0
if "%buildType%" == "dbg" set validBuildType=1
if "%buildType%" == "rel" set validBuildType=1
if "%buildType%" == "relDbg" set validBuildType=1
if "%buildType%" == "relMin" set validBuildType=1
if "%validBuildType%" == "0" (
  echo Error: "%buildType%" is not a valid build type.
  exit /b 1
)

REM Ensure that a target was given.
if "%target%" == "" (
  echo Error: A target must be specified.
  exit /b 1
)

exit /b 0
