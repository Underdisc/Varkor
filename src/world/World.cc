#include <sstream>

#include "World.h"

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
  defaultName << "Space " << newSpace;
  nSpaces.Emplace(defaultName.str());
  return newSpace;
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
  // Call the project update functions to give them control of processing.
  for (int i = 0; i < nSpaces.Size(); ++i)
  {
    SpaceId spaceId = i;
    if (SpaceUpdate != nullptr)
    {
      SpaceUpdate(nSpaces[i], spaceId);
    }
  }
  if (CentralUpdate != nullptr)
  {
    CentralUpdate();
  }
}

} // namespace World
