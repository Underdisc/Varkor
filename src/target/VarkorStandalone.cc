#include "VarkorMain.h"

int main(int argc, char* argv[]) {
  Options::Config config;
  config.mEditorLevel = Options::EditorLevel::Complete;
  config.mProjectDirectory = "";
  config.mWindowName = "Varkor Standalone";
  Result result = VarkorInit(argc, argv, std::move(config));
  if (!result.Success()) {
    return 0;
  }
  VarkorRun();
  VarkorPurge();
}
