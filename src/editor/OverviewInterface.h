#ifndef editor_OverviewInterface_h
#define editor_OverviewInterface_h

#include "editor/InspectorInterface.h"
#include "editor/WindowInterface.h"
#include "world/World.h"

namespace Editor {

struct OverviewInterface: public WindowInterface
{
public:
  OverviewInterface(World::LayerIt layerIt);
  void Show();
  World::LayerIt mLayerIt;

private:
  void DisplayMember(World::MemberId memberId, InspectorInterface** inspector);
};

} // namespace Editor

#endif
