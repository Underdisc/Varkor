#ifndef editor_LayerInterface_h
#define editor_LayerInterface_h

#include "editor/InspectorInterface.h"
#include "editor/Interface.h"
#include "world/World.h"

namespace Editor {

struct LayerInterface: public Interface {
public:
  LayerInterface(World::LayerIt layerIt);
  void Show();
  void ObjectPicking();
  void SaveLayer();
  void SaveLayerAs();
  void SaveLayerAs(const std::string& filename);
  World::LayerIt mLayerIt;

private:
  void DisplayMember(World::MemberId memberId, InspectorInterface** inspector);
};

} // namespace Editor

#endif
