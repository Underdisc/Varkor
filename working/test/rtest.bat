@echo off

REM Usage: rtest.bat {target} [c|d]

REM Required Arguments
REM {target} - The name of the target executable to run.

REM Optional Arguments
REM c - Creates a file containing the output of the target executable named
REM   {target}_out.txt. The file will be overwritten if it already exists.
REM d - Creates a file containing the output of the target executable
REM   named {target}_out_diff.txt and diffs it against {target}_out.txt if
REM   {target}_out.txt exists
REM Not providing c or d as arguments will print the output of the target
REM executable to console.

if "%1" == "" (
  echo Error: A target name must be provided as the first argument.
  goto:eof
)
if not exist %1.exe (
  echo Error: Target executable "%1.exe" does not exist.
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
%1.exe
