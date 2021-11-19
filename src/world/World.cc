#include <sstream>

#include "ds/Vector.h"
#include "util/Utility.h"
#include "vlk/Explorer.h"
#include "vlk/Value.h"
#include "world/Space.h"
#include "world/World.h"

namespace World {

// todo: This should be a list instead of a vector.
// This is just the list of the spaces that currently exist within the world.
Ds::Vector<Space> nSpaces;

SpaceVisitor::SpaceVisitor(): mCurrentSpace(0) {}

Space& SpaceVisitor::CurrentSpace()
{
  return nSpaces[mCurrentSpace];
}

SpaceId SpaceVisitor::CurrentSpaceId()
{
  return mCurrentSpace;
}

void SpaceVisitor::Next()
{
  ++mCurrentSpace;
}

bool SpaceVisitor::End()
{
  return mCurrentSpace >= nSpaces.Size();
}

// Function pointers for calling into project code.
void (*CentralUpdate)() = nullptr;
void (*SpaceUpdate)(const Space& space, SpaceId spaceId) = nullptr;

SpaceId CreateSpace()
{
  SpaceId newSpace = (SpaceId)nSpaces.Size();
  std::stringstream defaultName;
  defaultName << "Space" << newSpace;
  nSpaces.Emplace(defaultName.str());
  return newSpace;
}

Util::ValuedResult<SpaceId> LoadSpace(const char* filename)
{
  Vlk::Value rootVal;
  Util::Result result = rootVal.Read(filename);
  if (!result.Success())
  {
    return Util::ValuedResult<SpaceId>(Util::Move(result), nInvalidSpaceId);
  }
  SpaceId newSpaceId = World::CreateSpace();
  World::Space& newSpace = World::GetSpace(newSpaceId);
  Vlk::Explorer rootEx(rootVal);
  newSpace.Deserialize(rootEx);
  return Util::ValuedResult<SpaceId>(newSpaceId);
}

void VerifySpace(SpaceId space)
{
  LogAbortIf(
    space < 0 || space >= nSpaces.Size(), "The provided space does not exist.");
}

Space& GetSpace(SpaceId id)
{
  VerifySpace(id);
  return nSpaces[id];
}

void Update()
{
  for (int i = 0; i < nSpaces.Size(); ++i)
  {
    nSpaces[i].Update();
  }
}

} // namespace World
