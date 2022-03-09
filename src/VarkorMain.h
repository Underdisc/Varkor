#ifndef VarkorMain_h
#define VarkorMain_h

#include "Result.h"

Result VarkorInit(
  int argc, char* argv[], const char* windowName, const char* projectDirectory);
void VarkorRun();
void VarkorPurge();

#endif
