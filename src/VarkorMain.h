#ifndef VarkorMain_h
#define VarkorMain_h

#include "Options.h"
#include "Result.h"

Result VarkorInit(int argc, char* argv[], Options::Config&& config);
void VarkorRun();
void VarkorPurge();

#endif
