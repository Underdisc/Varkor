#ifndef core_World_h
#define core_World_h

#include "Camera.h"
#include "core/Space.h"
#include "ds/Vector.h"

namespace World {

// This is used to iterate through all of the spaces that are currently in the
// world.
struct SpaceVisitor
{
  SpaceVisitor();
  Space& CurrentSpace();
  SpaceRef CurrentSpaceRef();
  void Next();
  bool End();

private:
  SpaceRef mCurrentSpace;
};

extern void (*CentralUpdate)();
extern void (*SpaceUpdate)(const Space& space, SpaceRef spaceRef);

void Init();
SpaceRef CreateSpace();
Space& GetSpace(SpaceRef ref);
void Update();
void Render(const Camera& camera);

template<typename T>
void UpdateComponentType(const Space& space, SpaceRef spaceRef);

} // namespace World

#include "World.hh"

#endif
