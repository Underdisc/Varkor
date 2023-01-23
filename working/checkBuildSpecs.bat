REM Ensures that all environment variables needed for build scripts are set.
REM compilerDir - The compiler directory within root/build/.
REM buildType - The type of build to be created.
REM target - The target to build.

REM Ensure that a valid compiler was given.
if "%compiler%" == "" (
  echo Error: A compiler must be specified.
  exit /b 1
)
set validCompiler=0
if "%compiler%" == "msvc64" set validCompiler=1
if "%compiler%" == "clang64" set validCompiler=1
if "%validCompiler%" == "0" (
  echo Error: "%compiler%" is not a valid compiler.
  exit /b 1
)

REM Ensure that a valid configuration was given.
if "%configuration%" == "" (
  echo Error: A build type must be specified.
  exit /b 1
)
set validConfiguration=0
if "%configuration%" == "dbg" set validConfiguration=1
if "%configuration%" == "rel" set validConfiguration=1
if "%configuration%" == "relDbg" set validConfiguration=1
if "%configuration%" == "relMin" set validConfiguration=1
if "%validConfiguration%" == "0" (
  echo Error: "%configuration%" is not a valid configuration.
  exit /b 1
)

REM Ensure that a target was given.
if "%target%" == "" (
  echo Error: A target must be specified.
  exit /b 1
)

exit /b 0
