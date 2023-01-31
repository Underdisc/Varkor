#ifndef editor_ResourceInterface_h
#define editor_ResourceInterface_h

#include <imgui/imgui.h>

#include "editor/WindowInterface.h"
#include "vlk/Value.h"

namespace Editor {

struct ResourceInterface: public WindowInterface
{
  ResourceInterface(const ResId& id);
  ~ResourceInterface();
  void Show();

  ResId mResId;
};

} // namespace Editor

#endif