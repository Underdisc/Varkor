REM Ensure that the generator and buildDir environment variables are set and
REM that buildDir points to an existing directory.
call buildSpecs.bat
if "%generator%" == "" (
  echo Error: The environment variable "generator" must be set in
  echo working/buildSpecs.bat
  exit /b 1
)
if "%buildDir%" == "" (
  echo Error: The environment variable "buildDir" must be set in
  echo working/buildSpecs.bat.
  exit /b 1
)
if not exist %buildDir% (
  echo Error: The path specified in buildDir [%buildDir%] does not exist
  echo relative to working/buildSpecs.bat.
  exit /b 1
)
exit /b 0
