#include "world/Space.h"
#include "world/World.h"

#include "Object.h"

namespace World {

Object::Object(): mSpace(nInvalidSpaceId), mMember(nInvalidMemberId) {}

Object::Object(SpaceId space, MemberId member): mSpace(space), mMember(member)
{}

void Object::AddComponent(Comp::TypeId typeId) const
{
  Space& space = GetSpace(mSpace);
  space.AddComponent(typeId, mMember);
}

void Object::RemComponent(Comp::TypeId typeId) const
{
  Space& space = GetSpace(mSpace);
  space.RemComponent(typeId, mMember);
}

void* Object::GetComponent(Comp::TypeId typeId) const
{
  Space& space = GetSpace(mSpace);
  return space.GetComponent(typeId, mMember);
}

bool Object::HasComponent(Comp::TypeId typeId) const
{
  Space& space = GetSpace(mSpace);
  return space.HasComponent(typeId, mMember);
}

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
