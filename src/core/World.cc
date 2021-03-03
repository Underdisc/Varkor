#include <sstream>

#include "gfx/Renderer.h"

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

SpaceRef SpaceVisitor::CurrentSpaceRef()
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
void (*SpaceUpdate)(const Space& space, SpaceRef spaceRef) = nullptr;

void Init()
{
  Gfx::Renderer::Init();
}

SpaceRef CreateSpace()
{
  SpaceRef newSpace = (SpaceRef)nSpaces.Size();
  std::stringstream defaultName;
  defaultName << "Space " << newSpace;
  nSpaces.Emplace(defaultName.str());
  return newSpace;
}

void VerifySpace(SpaceRef space)
{
  LogAbortIf(
    space < 0 || space >= nSpaces.Size(), "The provided space does not exist.");
}

Space& GetSpace(SpaceRef ref)
{
  VerifySpace(ref);
  return nSpaces[ref];
}

void Update()
{
  // Call the project update functions to give them control of processing.
  for (int i = 0; i < nSpaces.Size(); ++i)
  {
    SpaceRef spaceRef = i;
    if (SpaceUpdate != nullptr)
    {
      SpaceUpdate(nSpaces[i], spaceRef);
    }
  }
  if (CentralUpdate != nullptr)
  {
    CentralUpdate();
  }
}

// todo: Every space should have its own camera.
void Render(const Camera& camera)
{
  for (int i = 0; i < nSpaces.Size(); ++i)
  {
    Space& space = nSpaces[i];
    Gfx::Renderer::Render(space, camera.WorldToCamera());
  }
}

} // namespace World
