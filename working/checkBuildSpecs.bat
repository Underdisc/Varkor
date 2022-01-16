REM Ensures that all environment variables needed for working/build.bat are set
REM and that buildDir points to an existing directory.

REM Required environment variables for working/build.bat.
REM buildDir - The path to the build dir relative to buildSpecs.bat.
REM generator - The identifier used to call the generator.
REM target - The target to build and potentially run.
REM   This must be "varkor" for the standalone version of the engine.

pushd ..\
call buildSpecs.bat
if "%buildDir%" == "" (
  echo Error: The "buildDir" environment variable must be set in buildSpecs.bat.
  popd
  exit /b 1
)
if not exist %buildDir% (
  echo Error: The path specified in the buildDir environment variable [%buildDir%] does not exist relative to buildSpecs.bat.
  popd
  exit /b 1
)
if "%generator%" == "" (
  echo Error: The "generator" environment variable must be set in buildSpecs.bat.
  popd
  exit /b 1
)
if "%target%" == "" (
  echo Error: The "target" environment variable must be set in buildSpecs.bat.
  popd
  exit /b 1
)
popd
exit /b 0
