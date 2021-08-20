#ifndef editor_WindowInterface_h
#define editor_WindowInterface_h

#include "editor/Interface.h"

namespace Editor {

struct WindowInterface: public Interface
{
public:
  virtual ~WindowInterface();
  void HandleInterfaces();
  virtual void Show() = 0;
};

} // namespace Editor

#endif
