#include "VarkorMain.h"

int main(int argc, char* argv[])
{
  Result result = VarkorInit(argc, argv, "Varkor Standalone", "");
  if (!result.Success()) {
    return 0;
  }
  VarkorRun();
  VarkorPurge();
}