#include <sstream>
#include <utility>

#include "Error.h"
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

Member::Member(): mFirstDescriptorId(nInvalidDescriptorId) {}

void Member::StartUse(DescriptorId firstDescId, const std::string& name)
{
  mFirstDescriptorId = firstDescId;
  mDescriptorCount = 0;
  mParent = nInvalidMemberId;
  mName = name;
}

void Member::EndUse()
{
  mFirstDescriptorId = nInvalidDescriptorId;
  mChildren.Clear();
}

bool Member::InUse() const
{
  return mFirstDescriptorId != nInvalidDescriptorId;
}

bool Member::HasParent() const
{
  return mParent != nInvalidMemberId;
}

bool Member::InUseRootMember() const
{
  return InUse() && !HasParent();
}

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

MemberId Member::Parent() const
{
  return mParent;
}

const Ds::Vector<MemberId>& Member::Children() const
{
  return mChildren;
}

Space::Space() {}

void Space::Clear()
{
  mTables.Clear();
  mMembers.Clear();
  mUnusedMemberIds.Clear();
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

    for (int i = 0; i < table.Size(); ++i) {
      if (table.ActiveIndex(i)) {
        currentObject.mMemberId = table.GetOwner(i);
        typeData.mVUpdate.Invoke(table[i], currentObject);
      }
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
  std::stringstream name;

  // If we have unused member ids, we use those for the new member before using
  // member ids that have yet to be used.
  if (mUnusedMemberIds.Size() > 0) {
    MemberId newMemberId = mUnusedMemberIds.Top();
    Member& newMember = mMembers[newMemberId];
    name << "Member" << newMemberId;
    newMember.StartUse(newDescId, name.str());
    mUnusedMemberIds.Pop();
    return newMemberId;
  }
  Member newMember;
  MemberId newMemberId = (MemberId)mMembers.Size();
  name << "Member" << newMemberId;
  newMember.StartUse(newDescId, name.str());
  mMembers.Push(std::move(newMember));
  return newMemberId;
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

MemberId Space::Duplicate(MemberId ogMemberId, bool duplicationRoot)
{
  // Duplicate all of the components from the original member.
  VerifyMemberId(ogMemberId);
  MemberId newMemberId = CreateMember();
  const Member& ogMember = mMembers[ogMemberId];
  mMembers[newMemberId].mName = ogMember.mName;
  DescriptorId ogDescId = ogMember.mFirstDescriptorId;
  while (ogDescId < ogMember.EndDescriptorId()) {
    ComponentDescriptor newDesc;
    const ComponentDescriptor& ogDesc = mDescriptorBin[ogDescId];
    newDesc.mTypeId = ogDesc.mTypeId;
    Table& table = mTables.Get(ogDesc.mTypeId);
    newDesc.mTableIndex = table.Duplicate(ogDesc.mTableIndex, newMemberId);
    AddDescriptorToMember(newMemberId, newDesc);
    ++ogDescId;
  }

  // Make the new member a child if we are duplicating a child and duplicate all
  // of the member's children.
  if (duplicationRoot && ogMember.HasParent()) {
    MakeParent(ogMember.Parent(), newMemberId);
  }
  for (MemberId childId : ogMember.mChildren) {
    MemberId newChildId = Duplicate(childId, false);
    MakeParent(newMemberId, newChildId);
  }
  return newMemberId;
}

MemberId Space::CreateChildMember(MemberId parentId)
{
  MemberId childId = CreateMember();
  MakeParent(parentId, childId);
  return childId;
}

void Space::DeleteMember(MemberId memberId)
{
  // Delete all of the member's children.
  VerifyMemberId(memberId);
  Member& member = mMembers[memberId];
  for (MemberId childId : member.mChildren) {
    DeleteMember(childId);
  }

  // Remove all of the member's components.
  DescriptorId descId = member.mFirstDescriptorId;
  while (descId < member.EndDescriptorId()) {
    ComponentDescriptor& desc = mDescriptorBin[descId];
    Table& table = mTables.Get(desc.mTypeId);
    table.Rem(desc.mTableIndex);
    desc.EndUse();
    ++descId;
  }

  // Remove the parent's reference to the child.
  if (member.mParent != nInvalidMemberId) {
    Member& parent = mMembers[member.mParent];
    size_t index = parent.mChildren.Find(memberId);
    if (index != parent.mChildren.Size()) {
      parent.mChildren.LazyRemove(index);
    }
  }

  member.EndUse();
  mUnusedMemberIds.Push(memberId);
}

void Space::MakeParent(MemberId parentId, MemberId childId)
{
  // Verify the existance of the parent and child members.
  VerifyMemberId(parentId);
  VerifyMemberId(childId);
  Member& parentMember = mMembers[parentId];
  Member& childMember = mMembers[childId];

  // If the child already has a parent, remove that parent relationship and
  // create a new one.
  if (childMember.HasParent()) {
    RemoveParent(childId);
  }
  parentMember.mChildren.Push(childId);
  childMember.mParent = parentId;
}

void Space::RemoveParent(MemberId childId)
{
  // Verify that the child exists and has a parent.
  VerifyMemberId(childId);
  Member& childMember = mMembers[childId];
  LogAbortIf(!childMember.HasParent(), "This member has no parent.");

  // End the relationship by removing the child from the parent's children and
  // removing the parent from the child.
  Member& parent = mMembers[childMember.mParent];
  for (size_t i = 0; i < parent.mChildren.Size(); ++i) {
    if (parent.mChildren[i] == childId) {
      parent.mChildren.LazyRemove(i);
      break;
    }
  }
  childMember.mParent = nInvalidMemberId;
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
  Table* table = mTables.Find(typeId);
  if (table == nullptr) {
    Table newTable(typeId);
    mTables.Insert(typeId, newTable);
    table = mTables.Find(typeId);
  }
  const Comp::TypeData& typeData = Comp::GetTypeData(typeId);
  if (HasComponent(typeId, memberId)) {
    const Member& member = mMembers[memberId];
    std::stringstream error;
    error << member.mName << " (MemberId: " << memberId << ") already has a "
          << typeData.mName << " (TypeId: " << typeId << ") component.";
    LogAbort(error.str().c_str());
  }

  // Add any missing dependencies.
  for (Comp::TypeId dependencyId : typeData.mDependencies) {
    if (!HasComponent(dependencyId, memberId)) {
      AddComponent(dependencyId, memberId, init);
    }
  }

  // Create the component's descriptor.
  ComponentDescriptor newDesc;
  newDesc.mTypeId = typeId;
  newDesc.mTableIndex = table->Add(memberId);
  AddDescriptorToMember(memberId, newDesc);

  // Initialize the component if requested.
  void* component = table->GetComponent(newDesc.mTableIndex);
  if (init && typeData.mVInit.Open()) {
    Object owner(this, memberId);
    typeData.mVInit.Invoke(component, owner);
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
    error << member.mName << " (MemberId: " << memberId << ") does not have a "
          << typeData.mName << " (TypeId: " << typeId << ") component.";
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
  table.Rem(desc.mTableIndex);
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
    error << member.mName << " (" << memberId << ") did not contain a "
          << typeData.mName << " (" << typeId << ") component.";
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
      return table[desc.mTableIndex];
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
  Table* table = mTables.Find(typeId);
  if (table == nullptr) {
    return members;
  }
  for (int i = 0; i < table->Size(); ++i) {
    if (table->ActiveIndex(i)) {
      members.Push(table->GetOwner(i));
    }
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
  for (MemberId i = 0; i < mMembers.Size(); ++i) {
    const Member& member = mMembers[i];
    if (member.InUseRootMember()) {
      rootMembers.Push(i);
    }
  }
  return rootMembers;
}

const Ds::Map<Comp::TypeId, Table>& Space::Tables() const
{
  return mTables;
}

const Ds::Vector<Member>& Space::Members() const
{
  return mMembers;
}

const Ds::Vector<MemberId>& Space::UnusedMemberIds() const
{
  return mUnusedMemberIds;
}

const Ds::Vector<ComponentDescriptor> Space::DescriptorBin() const
{
  return mDescriptorBin;
}

void Space::Serialize(Vlk::Value& spaceVal) const
{
  for (MemberId memberId = 0; memberId < mMembers.Size(); ++memberId) {
    // We only handle members that are in use.
    const Member& member = mMembers[memberId];
    if (!member.InUse()) {
      continue;
    }

    // Serialize all of the member's data.
    Vlk::Value& memberVal = spaceVal(member.mName.c_str());
    memberVal("Id") = memberId;
    memberVal("Parent") = member.mParent;
    Vlk::Value& childrenVal = memberVal("Children")[{member.mChildren.Size()}];
    for (size_t i = 0; i < member.mChildren.Size(); ++i) {
      childrenVal[i] = member.mChildren[i];
    }
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
      typeData.mVSerialize.Invoke(table[desc.mTableIndex], componentVal);
    }
  }
}

Result Space::Deserialize(const Vlk::Explorer& spaceEx)
{
  for (size_t i = 0; i < spaceEx.Size(); ++i) {
    // Ensure that the current member is valid.
    Vlk::Explorer memberEx = spaceEx(i);
    MemberId memberId = memberEx("Id").As<int>(nInvalidMemberId);
    if (memberId == nInvalidMemberId) {
      LogError("A Member should have a valid Id.");
      continue;
    }
    World::Object owner(this, memberId);

    // Deserialize the member's data.
    if (memberId >= mMembers.Size()) {
      mMembers.Resize(memberId + 1);
    }
    Member& member = mMembers[memberId];
    member.StartUse((DescriptorId)mDescriptorBin.Size(), memberEx.Key());
    member.mParent = memberEx("Parent").As<int>(nInvalidMemberId);
    Vlk::Explorer childrenEx = memberEx("Children");
    for (size_t i = 0; i < childrenEx.Size(); ++i) {
      member.mChildren.Push(childrenEx[i].As<int>());
    }
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
  return memberId >= 0 && memberId < mMembers.Size() &&
    mMembers[memberId].InUse();
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
