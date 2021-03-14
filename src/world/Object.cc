#include "world/World.h"

#include "Object.h"

namespace World {

Object::Object(): mSpace(nInvalidSpaceId), mMember(nInvalidMemberId) {}

Object::Object(SpaceId space, MemberId member): mSpace(space), mMember(member)
{}

std::string& Object::GetName() const
{
  return GetSpace(mSpace).mMembers[mMember].mName;
}

bool Object::HasParent() const
{
  return GetSpace(mSpace).mMembers[mMember].HasParent();
}

Object Object::Parent() const
{
  Object parent(mSpace, GetSpace(mSpace).mMembers[mMember].Parent());
  return parent;
}

void Object::Invalidate()
{
  mSpace = nInvalidSpaceId;
  mMember = nInvalidMemberId;
}

bool Object::Valid() const
{
  return mSpace != nInvalidSpaceId && mMember != nInvalidMemberId;
}

} // namespace World
