#include <sstream>

#include "World.h"

namespace Core {
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

Object::Object(): mSpace(nInvalidSpaceRef), mMember(nInvalidMemRef) {}

Object::Object(SpaceRef space, MemRef member): mSpace(space), mMember(member) {}

std::string& Object::GetName()
{
  return nSpaces[mSpace].mMembers[mMember].mName;
}

void Object::Invalidate()
{
  mSpace = nInvalidSpaceRef;
  mMember = nInvalidMemRef;
}

bool Object::Valid()
{
  return mSpace != nInvalidSpaceRef && mMember != nInvalidMemRef;
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

} // namespace World
} // namespace Core
