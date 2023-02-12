#ifndef editor_ErrorInterface_h
#define editor_ErrorInterface_h

#include "editor/Interface.h"

namespace Editor {

struct ErrorInterface: public Interface
{
  void Show();
};

} // namespace Editor

#endif
