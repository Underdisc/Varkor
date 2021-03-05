#ifndef world_World_h
#define world_World_h

#include "Camera.h"
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

void Init();
SpaceId CreateSpace();
Space& GetSpace(SpaceId id);
void Update();
void Render(const Camera& camera);

template<typename T>
void UpdateComponentType(const Space& space, SpaceId spaceId);

} // namespace World

#include "World.hh"

#endif
