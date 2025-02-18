#ifndef Options_h
#define Options_h

#include <string>

#include "Result.h"
#include "ds/Vector.h"

namespace Options {

enum class EditorLevel {
  // Allows the creation of windows. No other windows are shown and no other
  // editor features are available.
  Simple = 0,
  // Shows the entire editor.
  Complete = 1,
};

struct Config {
  std::string mWindowName;
  std::string mProjectDirectory;
  EditorLevel mEditorLevel;
  Ds::Vector<std::string> mLoadLayers;
};

extern Config nConfig;
Result Init(int argc, char* argv[], Config&& config);

}; // namespace Options

#endif
