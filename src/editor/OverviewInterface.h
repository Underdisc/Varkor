#ifndef editor_OverviewInterface_h
#define editor_OverviewInterface_h

#include "editor/InspectorInterface.h"
#include "editor/WindowInterface.h"
#include "world/Space.h"
#include "world/Types.h"

namespace Editor {

struct OverviewInterface: public WindowInterface
{
public:
  OverviewInterface(World::SpaceId spaceId);
  void Show();
  World::SpaceId mSpaceId;

private:
  void DisplayMember(
    World::MemberId memberId,
    World::Space& space,
    InspectorInterface* inspector);
};

} // namespace Editor

#endif
