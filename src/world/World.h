#ifndef world_World_h
#define world_World_h

#include "world/Space.h"
#include "world/Types.h"

namespace World {

// This is used to iterate through all of the spaces that are currently in the
// world.
struct SpaceVisitor
{
  SpaceVisitor();
  Space& CurrentSpace();
  SpaceId CurrentSpaceId();
  void Next();
  bool End();

private:
  SpaceId mCurrentSpace;
};

SpaceId CreateSpace();
Util::ValuedResult<SpaceId> LoadSpace(const char* filename);
Space& GetSpace(SpaceId id);
void Update();

} // namespace World

#endif
