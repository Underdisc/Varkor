#ifndef editor_ResourceInterface_h
#define editor_ResourceInterface_h

#include <imgui/imgui.h>

#include "editor/WindowInterface.h"
#include "vlk/Value.h"

namespace Editor {

struct ResourceInterface: public WindowInterface
{
  ResourceInterface(const ResId& id);
  void Show();

  ResId mResId;
  // We overwrite the asset file when these two are no longer the same.
  Vlk::Value mAssetVal;
  Vlk::Value mAssetValCopy;
};

} // namespace Editor

#endif