#ifndef world_World_h
#define world_World_h

#include "ds/Vector.h"
#include "world/Space.h"

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

extern void (*CentralUpdate)();
extern void (*SpaceUpdate)(const Space& space, SpaceId spaceId);

SpaceId CreateSpace();
Space& GetSpace(SpaceId id);
void Update();

template<typename T>
void UpdateComponentType(const Space& space, SpaceId spaceId);

} // namespace World

#include "World.hh"

#endif
