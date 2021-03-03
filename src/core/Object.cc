#include "core/World.h"

#include "Object.h"

namespace World {

Object::Object(): mSpace(nInvalidSpaceRef), mMember(nInvalidMemRef) {}

Object::Object(SpaceRef space, MemRef member): mSpace(space), mMember(member) {}

std::string& Object::GetName() const
{
  return GetSpace(mSpace).mMembers[mMember].mName;
}

void Object::Invalidate()
{
  mSpace = nInvalidSpaceRef;
  mMember = nInvalidMemRef;
}

bool Object::Valid() const
{
  return mSpace != nInvalidSpaceRef && mMember != nInvalidMemRef;
}

} // namespace World
