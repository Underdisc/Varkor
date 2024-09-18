#include <sstream>
#include <utility>

#include "Error.h"
#include "comp/Name.h"
#include "comp/Relationship.h"
#include "vlk/Valkor.h"
#include "world/Object.h"
#include "world/Space.h"

namespace World {

void ComponentDescriptor::EndUse()
{
  mTypeId = Comp::nInvalidTypeId;
}

bool ComponentDescriptor::InUse() const
{
  return mTypeId != Comp::nInvalidTypeId;
}

Member::Member(): mFirstDescriptorId(nInvalidDescriptorId), mDescriptorCount(0)
{}

Member::Member(DescriptorId FirstDescriptorId):
  mFirstDescriptorId(FirstDescriptorId), mDescriptorCount(0)
{}

DescriptorId Member::EndDescriptorId() const
{
  return mFirstDescriptorId + mDescriptorCount;
}

DescriptorId Member::LastDescriptorId() const
{
  return mFirstDescriptorId + mDescriptorCount - 1;
}

DescriptorId Member::FirstDescriptorId() const
{
  return mFirstDescriptorId;
}

DescriptorId Member::DescriptorCount() const
{
  return mDescriptorCount;
}

Space::Space() {}

void Space::Clear()
{
  mTables.Clear();
  mMembers.Clear();
  mDescriptorBin.Clear();
}

void Space::Update()
{
  Object currentObject(this);
  for (const Ds::KvPair<Comp::TypeId, Table>& tablePair : mTables) {
    const Table& table = tablePair.mValue;
    const Comp::TypeData& typeData = Comp::GetTypeData(table.TypeId());
    if (!typeData.mVUpdate.Open()) {
      continue;
    }

    for (size_t i = 0; i < table.Size(); ++i) {
      currentObject.mMemberId = table.GetOwnerAtDenseIndex(i);
      void* component = table.GetComponentAtDenseIndex(i);
      typeData.mVUpdate.Invoke(component, currentObject);
    }
  }
}

MemberId Space::CreateMember()
{
  // todo: The new ComponentDescriptor is always at the end of the descriptor
  // bin. This ignores unused memory within the descriptor bin. Without garbage
  // collection, this will result in a perpetually increasing memory overhead.
  // We should be more smart about what descriptor we hand out for a new member
  // in the future.
  DescriptorId newDescId = (DescriptorId)mDescriptorBin.Size();
  Member newMember(newDescId);
  return mMembers.Add(std::move(newMember));
}

Object Space::CreateObject()
{
  return Object(this, CreateMember());
}

Object Space::CreateChildObject(const Object& parent)
{
  LogAbortIf(this != parent.mSpace, "The parent points to a different space.");
  return Object(this, CreateChildMember(parent.mMemberId));
}

Object Space::CreateChildObject(MemberId parentId)
{
  return Object(this, CreateChildMember(parentId));
}

MemberId Space::Duplicate(MemberId memberId, bool root)
{
  // Duplicate all components from the member except the relationship component.
  VerifyMemberId(memberId);
  MemberId newMemberId = CreateMember();
  const Member& member = mMembers[memberId];
  DescriptorId descId = member.mFirstDescriptorId;
  for (; descId < member.EndDescriptorId(); ++descId) {
    const ComponentDescriptor& desc = mDescriptorBin[descId];
    if (desc.mTypeId == Comp::Type<Comp::Relationship>::smId) {
      continue;
    }
    ComponentDescriptor newDesc;
    newDesc.mTypeId = desc.mTypeId;
    Table& table = mTables.Get(desc.mTypeId);
    newDesc.mComponentId = table.Duplicate(desc.mComponentId, newMemberId);
    AddDescriptorToMember(newMemberId, newDesc);
  }

  // Make the new member a child if we are duplicating a child and duplicate all
  // of the member's children.
  auto* relationship = TryGet<Comp::Relationship>(memberId);
  if (relationship != nullptr) {
    if (root && relationship->HasParent()) {
      MakeParent(relationship->mParent, newMemberId);
      relationship = &Get<Comp::Relationship>(memberId);
    }
    for (MemberId childId : relationship->mChildren) {
      MemberId newChildId = Duplicate(childId, false);
      relationship = &Get<Comp::Relationship>(memberId);
      MakeParent(newMemberId, newChildId);
      relationship = &Get<Comp::Relationship>(memberId);
    }
  }
  return newMemberId;
}

MemberId Space::CreateChildMember(MemberId parentId)
{
  MemberId childId = CreateMember();
  MakeParent(parentId, childId);
  return childId;
}

void Space::DeleteMember(MemberId memberId, bool root)
{
  // Delete all of the member's children and if there is parent remove the
  // member's id from the parent's relationship .
  auto* relationship = TryGet<Comp::Relationship>(memberId);
  if (relationship != nullptr) {
    for (int i = 0; i < relationship->mChildren.Size(); ++i) {
      DeleteMember(relationship->mChildren[i], false);
    }
    if (root && relationship->HasParent()) {
      auto& parentRelationship = Get<Comp::Relationship>(relationship->mParent);
      parentRelationship.NullifyChild(memberId);
      if (!parentRelationship.HasRelationship()) {
        Rem<Comp::Relationship>(relationship->mParent);
      }
    }
  }

  // Remove all of the member's components.
  Member& member = mMembers[memberId];
  DescriptorId descId = member.mFirstDescriptorId;
  while (descId < member.EndDescriptorId()) {
    ComponentDescriptor& desc = mDescriptorBin[descId];
    Table& table = mTables.Get(desc.mTypeId);
    table.Remove(desc.mComponentId);
    desc.EndUse();
    ++descId;
  }

  mMembers.Remove(memberId);
}

void Space::TryDeleteMember(MemberId memberId)
{
  if (ValidMemberId(memberId)) {
    DeleteMember(memberId);
  }
}

void Space::MakeParent(MemberId parentId, MemberId childId)
{
  // Verify the existence of the parent and child members.
  VerifyMemberId(parentId);
  VerifyMemberId(childId);

  // Remove the childId from the parent's relationship, remove the parent's
  // relationship component if it no longer has relationships, and update the
  // child's parent id.
  auto& childRelationship = Ensure<Comp::Relationship>(childId);
  if (childRelationship.HasParent()) {
    auto& oldParentRelationship =
      Get<Comp::Relationship>(childRelationship.mParent);
    oldParentRelationship.NullifyChild(childId);
    if (!oldParentRelationship.HasRelationship()) {
      Rem<Comp::Relationship>(parentId);
    }
  }
  childRelationship.mParent = parentId;

  // Ensure that the new parent has a relationship component and add the child
  // id to the new parent's child vector.
  auto& newParentRelationship = Ensure<Comp::Relationship>(parentId);
  newParentRelationship.mChildren.Push(childId);
}

void Space::TryRemoveParent(MemberId memberId)
{
  VerifyMemberId(memberId);
  auto* relationship = TryGet<Comp::Relationship>(memberId);
  if (relationship == nullptr) {
    return;
  }
  if (!relationship->HasParent()) {
    return;
  }

  // Remove the child's id from the parent's children vector and the parent id
  // from child's relationship.
  auto& parentRelationship = Get<Comp::Relationship>(relationship->mParent);
  parentRelationship.NullifyChild(memberId);
  relationship->NullifyParent();

  // Remove no longer needed relationship components.
  if (!parentRelationship.HasRelationship()) {
    Rem<Comp::Relationship>(relationship->mParent);
  }
  if (!relationship->HasChildren()) {
    Rem<Comp::Relationship>(memberId);
  }
}

bool Space::HasParent(MemberId memberId)
{
  auto* relationship = TryGet<Comp::Relationship>(memberId);
  return relationship != nullptr && relationship->HasParent();
}

bool Space::HasChildren(MemberId memberId)
{
  auto* relationship = TryGet<Comp::Relationship>(memberId);
  return relationship != nullptr && !relationship->mChildren.Empty();
}

Member& Space::GetMember(MemberId id)
{
  VerifyMemberId(id);
  return mMembers[id];
}

const Member& Space::GetConstMember(MemberId id) const
{
  VerifyMemberId(id);
  return mMembers[id];
}

void* Space::AddComponent(Comp::TypeId typeId, MemberId memberId, bool init)
{
  // Create the component table if necessary and make sure the member doesn't
  // already have the component.
  Table* table = mTables.TryGet(typeId);
  if (table == nullptr) {
    Table newTable(typeId);
    mTables.Insert(typeId, newTable);
    table = mTables.TryGet(typeId);
  }
  const Comp::TypeData& typeData = Comp::GetTypeData(typeId);
  if (HasComponent(typeId, memberId)) {
    std::stringstream error;
    error << "Member " << memberId;
    Comp::Name* nameComp = TryGetComponent<Comp::Name>(memberId);
    if (nameComp != nullptr) {
      error << " (" << nameComp->mName << ")";
    }
    error << " already has a " << typeData.mName << " (TypeId: " << typeId
          << ") component.";
    LogAbort(error.str().c_str());
  }

  // Add any missing dependencies.
  for (Comp::TypeId dependencyId : typeData.mDependencies) {
    if (!HasComponent(dependencyId, memberId)) {
      void* depedency = AddComponent(dependencyId, memberId, init);
      const Comp::TypeData& dependencyTypeData =
        Comp::GetTypeData(dependencyId);
      if (dependencyTypeData.mVInit.Open()) {
        const World::Object owner(this, dependencyId);
        dependencyTypeData.mVInit.Invoke(depedency, owner);
      }
    }
  }

  // Create the component's descriptor.
  ComponentDescriptor newDesc;
  newDesc.mTypeId = typeId;
  newDesc.mComponentId = table->Add(memberId);
  AddDescriptorToMember(memberId, newDesc);

  // Initialize the component if requested.
  void* component = table->GetComponent(newDesc.mComponentId);
  if (init && typeData.mVInit.Open()) {
    Object owner(this, memberId);
    typeData.mVInit.Invoke(component, owner);
  }
  return component;
}

void* Space::EnsureComponent(Comp::TypeId typeId, MemberId memberId)
{
  void* component = TryGetComponent(typeId, memberId);
  if (component == nullptr) {
    component = AddComponent(typeId, memberId);
  }
  return component;
}

void Space::AddDescriptorToMember(
  MemberId memberId, const ComponentDescriptor& descriptor)
{
  Member& member = mMembers[memberId];
  DescriptorId nextDescId = member.EndDescriptorId();
  if (nextDescId >= mDescriptorBin.Size()) {
    mDescriptorBin.Push(descriptor);
    ++member.mDescriptorCount;
    return;
  }
  if (!mDescriptorBin[nextDescId].InUse()) {
    mDescriptorBin[nextDescId] = descriptor;
    ++member.mDescriptorCount;
    return;
  }
  for (size_t i = 0; i < member.mDescriptorCount; ++i) {
    DescriptorId ogDescId = member.mFirstDescriptorId + (DescriptorId)i;
    ComponentDescriptor ogDesc = mDescriptorBin[ogDescId];
    mDescriptorBin.Push(ogDesc);
    mDescriptorBin[ogDescId].EndUse();
  }
  mDescriptorBin.Push(descriptor);
  ++member.mDescriptorCount;
  member.mFirstDescriptorId =
    (DescriptorId)mDescriptorBin.Size() - member.mDescriptorCount;
}

void Space::RemComponent(Comp::TypeId typeId, MemberId memberId)
{
  // Find the ComponentDescriptor for the member's component.
  VerifyMemberId(memberId);
  Member& member = mMembers[memberId];
  DescriptorId descId = member.mFirstDescriptorId;
  DescriptorId endDescId = member.EndDescriptorId();
  while (descId < endDescId) {
    if (mDescriptorBin[descId].mTypeId == typeId) {
      break;
    }
    ++descId;
  }
  const Comp::TypeData& typeData = Comp::GetTypeData(typeId);
  if (descId == endDescId) {
    std::stringstream error;
    error << "Member " << memberId;
    Comp::Name* nameComp = TryGetComponent<Comp::Name>(memberId);
    if (nameComp != nullptr) {
      error << " (" << nameComp->mName << ")";
    }
    error << " does not have a " << typeData.mName << " (TypeId: " << typeId
          << ") component.";
    LogAbort(error.str().c_str());
  }

  // Remove all of the dependant components.
  for (Comp::TypeId dependantId : typeData.mDependants) {
    if (HasComponent(dependantId, memberId)) {
      RemComponent(dependantId, memberId);
    }
  }

  // Remove the old ComponentDescriptor from the member.
  Table& table = mTables.Get(typeId);
  ComponentDescriptor& desc = mDescriptorBin[descId];
  table.Remove(desc.mComponentId);
  if (descId == member.LastDescriptorId()) {
    desc.EndUse();
  }
  else {
    ComponentDescriptor& lastDesc = mDescriptorBin[member.LastDescriptorId()];
    desc = lastDesc;
    lastDesc.EndUse();
  }
  --member.mDescriptorCount;
}

void* Space::GetComponent(Comp::TypeId typeId, MemberId memberId) const
{
  VerifyMemberId(memberId);
  void* component = TryGetComponent(typeId, memberId);
  if (component == nullptr) {
    const Member& member = mMembers[memberId];
    const Comp::TypeData& typeData = Comp::GetTypeData(typeId);
    std::stringstream error;
    error << "Member " << memberId;
    Comp::Name* nameComp = TryGetComponent<Comp::Name>(memberId);
    if (nameComp != nullptr) {
      error << " (" << nameComp->mName << ")";
    }
    error << " did not contain a " << typeData.mName << " (" << typeId
          << ") component.";
    LogAbort(error.str().c_str());
  }
  return component;
}

void* Space::TryGetComponent(Comp::TypeId typeId, MemberId memberId) const
{
  if (!ValidMemberId(memberId)) {
    return nullptr;
  }
  const Member& member = mMembers[memberId];
  for (size_t i = 0; i < member.mDescriptorCount; ++i) {
    const ComponentDescriptor& desc =
      mDescriptorBin[member.mFirstDescriptorId + i];
    if (desc.mTypeId == typeId) {
      Table& table = mTables.Get(typeId);
      return table.GetComponent(desc.mComponentId);
    }
  }
  return nullptr;
}

bool Space::HasComponent(Comp::TypeId typeId, MemberId memberId) const
{
  if (!ValidMemberId(memberId)) {
    return false;
  }
  void* component = TryGetComponent(typeId, memberId);
  return component != nullptr;
}

Ds::Vector<MemberId> Space::Slice(Comp::TypeId typeId) const
{
  Ds::Vector<MemberId> members;
  Table* table = mTables.TryGet(typeId);
  if (table == nullptr) {
    return members;
  }
  for (size_t i = 0; i < table->Size(); ++i) {
    members.Push(table->GetOwnerAtDenseIndex(i));
  }
  return members;
}

Ds::Vector<ComponentDescriptor> Space::GetDescriptors(MemberId memberId) const
{
  VerifyMemberId(memberId);
  Ds::Vector<ComponentDescriptor> descriptors;
  const Member& member = mMembers[memberId];
  DescriptorId descId = member.mFirstDescriptorId;
  while (descId < member.EndDescriptorId()) {
    descriptors.Push(mDescriptorBin[descId]);
    ++descId;
  }
  return descriptors;
}

Ds::Vector<MemberId> Space::RootMemberIds() const
{
  Ds::Vector<MemberId> rootMembers;
  for (size_t i = 0; i < mMembers.DenseUsage(); ++i) {
    MemberId memberId = mMembers.Dense()[i];
    auto* relationship = TryGet<Comp::Relationship>(memberId);
    if (relationship == nullptr || !relationship->HasParent()) {
      rootMembers.Push(memberId);
    }
  }
  return rootMembers;
}

const Ds::Map<Comp::TypeId, Table>& Space::Tables() const
{
  return mTables;
}

const Ds::Pool<Member>& Space::Members() const
{
  return mMembers;
}

const Ds::Vector<ComponentDescriptor>& Space::DescriptorBin() const
{
  return mDescriptorBin;
}

void Space::Serialize(Vlk::Value& spaceVal) const
{
  for (int i = 0; i < mMembers.DenseUsage(); ++i) {
    // Serialize all of the member's data.
    const Member& member = mMembers.GetWithDenseIndex(i);
    Vlk::Value memberVal;
    Ds::PoolId memberId = mMembers.Dense()[i];
    memberVal("Id") = memberId;
    Vlk::Value& componentsVal = memberVal("Components");
    Ds::Vector<ComponentDescriptor> descriptors = GetDescriptors(memberId);
    for (int i = 0; i < descriptors.Size(); ++i) {
      const ComponentDescriptor& desc = descriptors[i];
      const Comp::TypeData& typeData = Comp::nTypeData[desc.mTypeId];
      Vlk::Value& componentVal = componentsVal(typeData.mName);
      if (!typeData.mVSerialize.Open()) {
        continue;
      }
      Table& table = mTables.Get(desc.mTypeId);
      void* component = table.GetComponent(desc.mComponentId);
      typeData.mVSerialize.Invoke(component, componentVal);
    }
    spaceVal.EmplaceValue(std::move(memberVal));
  }
}

Result Space::Deserialize(const Vlk::Explorer& spaceEx)
{
  if (!spaceEx.Valid(Vlk::Value::Type::ValueArray)) {
    return Result("Space Value must be a ValueArray");
  }

  for (size_t i = 0; i < spaceEx.Size(); ++i) {
    // Create the member.
    Vlk::Explorer memberEx = spaceEx[i];
    MemberId memberId = memberEx("Id").As<int>(nInvalidMemberId);
    if (memberId == nInvalidMemberId) {
      return Result("Member at " + memberEx.Path() + " has an invalid Id.");
    }
    Member& member = mMembers.Request(memberId);
    member.mFirstDescriptorId = (DescriptorId)mDescriptorBin.Size();

    // Get the member's component data.
    World::Object owner(this, memberId);
    Vlk::Explorer componentsEx = memberEx("Components");
    for (size_t i = 0; i < componentsEx.Size(); ++i) {
      Vlk::Explorer componentEx = componentsEx(i);
      Comp::TypeId typeId = Comp::GetTypeId(componentEx.Key());
      if (typeId == Comp::nInvalidTypeId) {
        return Result(
          "Component type \"" + componentEx.Key() + "\" at " +
          componentEx.Path() + " isn't a valid type.");
      }
      void* component = TryGetComponent(typeId, memberId);
      if (component == nullptr) {
        component = AddComponent(typeId, memberId, false);
      }
      const Comp::TypeData& typeData = Comp::GetTypeData(typeId);
      if (typeData.mVDeserialize.Open()) {
        typeData.mVDeserialize.Invoke(component, componentEx);
      }
      else if (typeData.mVInit.Open()) {
        typeData.mVInit.Invoke(component, owner);
      }
    }
  }
  return Result();
}

bool Space::ValidMemberId(MemberId memberId) const
{
  return mMembers.Valid(memberId);
}

void Space::VerifyMemberId(MemberId memberId) const
{
  if (ValidMemberId(memberId)) {
    return;
  }
  std::stringstream error;
  error << "There is no member with id " << memberId;
  LogAbort(error.str().c_str());
}

} // namespace World
