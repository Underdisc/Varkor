#ifndef editor_TempInterface_h
#define editor_TempInterface_h

#include "editor/WindowInterface.h"

namespace Editor {

// Use this to quickly test editing properties without needing to make a
// deditcated interface or repurposing part of another interface.
struct TempInterface: public WindowInterface
{
  void Show();
};

} // namespace Editor

#endif