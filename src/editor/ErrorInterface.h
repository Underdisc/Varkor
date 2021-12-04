#ifndef editor_ErrorInterface_h
#define editor_ErrorInterface_h

#include "editor/WindowInterface.h"

namespace Editor {

struct ErrorInterface: public WindowInterface
{
  void Show();
};

} // namespace Editor

#endif
