#ifndef editor_ResourceInterface_h
#define editor_ResourceInterface_h

#include <imgui/imgui.h>

#include "editor/Interface.h"
#include "vlk/Value.h"

namespace Editor {

struct ResourceInterface: public Interface {
  ResourceInterface(const ResId& id);
  ~ResourceInterface();
  void Show();

  ResId mResId;
};

} // namespace Editor

#endif
