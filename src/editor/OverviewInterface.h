#ifndef editor_OverviewInterface_h
#define editor_OverviewInterface_h

#include "editor/InspectorInterface.h"
#include "editor/WindowInterface.h"
#include "world/Space.h"

namespace Editor {

struct OverviewInterface: public WindowInterface
{
public:
  OverviewInterface(World::Space* space);
  void Show();
  World::Space* mSpace;

private:
  void DisplayMember(World::MemberId memberId, InspectorInterface** inspector);
};

} // namespace Editor

#endif
