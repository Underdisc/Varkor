#ifndef world_World_h
#define world_World_h

#include "Result.h"
#include "ds/List.h"
#include "world/Space.h"
#include "world/Types.h"

namespace World {

extern Ds::List<Space> nSpaces;
typedef Ds::List<Space>::Iter SpaceIt;

extern void (*nCentralUpdate)();
extern void (*nSpaceUpdate)(SpaceIt spaceIt);

void Purge();
void Update();
SpaceIt CreateTopSpace();
void DeleteSpace(SpaceIt it);
ValueResult<SpaceIt> LoadSpace(const char* filename);

} // namespace World

#endif
