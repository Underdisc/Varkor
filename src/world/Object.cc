#include "Object.h"

namespace World {

Object::Object(): mSpace(nullptr), mMemberId(nInvalidMemberId) {}

Object::Object(const Object& other):
  mSpace(other.mSpace), mMemberId(other.mMemberId)
{}

Object::Object(Space* space): mSpace(space) {}

Object::Object(Space* space, MemberId memberId):
  mSpace(space), mMemberId(memberId)
{}

void* Object::AddComponent(Comp::TypeId typeId) const
{
  return mSpace->AddComponent(typeId, mMemberId);
}

void Object::RemComponent(Comp::TypeId typeId) const
{
  mSpace->RemComponent(typeId, mMemberId);
}

void* Object::GetComponent(Comp::TypeId typeId) const
{
  return mSpace->GetComponent(typeId, mMemberId);
}

void* Object::TryGetComponent(Comp::TypeId typeId) const
{
  return mSpace->TryGetComponent(typeId, mMemberId);
}

bool Object::HasComponent(Comp::TypeId typeId) const
{
  return mSpace->HasComponent(typeId, mMemberId);
}

Object Object::CreateChild() const
{
  MemberId childId = mSpace->CreateChildMember(mMemberId);
  return Object(mSpace, childId);
}

Object Object::Duplicate() const
{
  MemberId newMemberId = mSpace->Duplicate(mMemberId);
  return Object(mSpace, newMemberId);
}

Member& Object::GetMember() const
{
  return mSpace->mMembers[mMemberId];
}

std::string& Object::GetName() const
{
  return mSpace->mMembers[mMemberId].mName;
}

const Ds::Vector<World::MemberId>& Object::Children() const
{
  return mSpace->mMembers[mMemberId].Children();
}

bool Object::HasParent() const
{
  return mSpace->mMembers[mMemberId].HasParent();
}

Object Object::Parent() const
{
  Object parent(mSpace, mSpace->mMembers[mMemberId].Parent());
  return parent;
}

bool Object::Valid() const
{
  return mSpace != nullptr && mMemberId != nInvalidMemberId;
}

} // namespace World
