#include "world/World.h"

#include "Object.h"

namespace World {

Object::Object(): mSpace(nInvalidSpaceId), mMember(nInvalidMemRef) {}

Object::Object(SpaceId space, MemRef member): mSpace(space), mMember(member) {}

std::string& Object::GetName() const
{
  return GetSpace(mSpace).mMembers[mMember].mName;
}

void Object::Invalidate()
{
  mSpace = nInvalidSpaceId;
  mMember = nInvalidMemRef;
}

bool Object::Valid() const
{
  return mSpace != nInvalidSpaceId && mMember != nInvalidMemRef;
}

} // namespace World
