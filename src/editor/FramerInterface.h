#ifndef editor_FramerInterface_h
#define editor_FramerInterface_h

#include "editor/WindowInterface.h"

namespace Editor {

struct FramerInterface: public WindowInterface
{
  void Show();
};

} // namespace Editor

#endif
