#ifndef world_World_h
#define world_World_h

#include "Result.h"
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

extern void (*nCentralUpdate)();
extern void (*nSpaceUpdate)(const Space& space, SpaceId spaceId);

SpaceId CreateSpace();
ValueResult<SpaceId> LoadSpace(const char* filename);
Space& GetSpace(SpaceId id);
void Update();
void Purge();

} // namespace World

#endif
