#ifndef editor_LayerInterface_h
#define editor_LayerInterface_h

#include "editor/InspectorInterface.h"
#include "editor/WindowInterface.h"
#include "world/World.h"

namespace Editor {

struct LayerInterface: public WindowInterface
{
public:
  LayerInterface(World::LayerIt layerIt);
  void Show();
  World::LayerIt mLayerIt;

private:
  void DisplayMember(World::MemberId memberId, InspectorInterface** inspector);
};

} // namespace Editor

#endif
