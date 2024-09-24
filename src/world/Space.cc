#include <sstream>
#include <utility>

#include "Error.h"
#include "comp/Name.h"
#include "comp/Relationship.h"
#include "vlk/Valkor.h"
#include "world/Object.h"
#include "world/Space.h"

namespace World {

Space::Space() {}

void Space::Clear()
{
  mTables.Clear();
  mMembers.Clear();
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
  return mMembers.Add();
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
  MemberId duplicateMemberId = CreateMember();
  for (auto it = mTables.begin(); it != mTables.end(); ++it) {
    Table& table = it->mValue;
    if (table.TypeId() == Comp::Type<Comp::Relationship>::smId) {
      continue;
    }
    if (!table.ValidComponent(memberId)) {
      continue;
    }
    table.Duplicate(memberId, duplicateMemberId);
  }

  // Make the new member a child if we are duplicating a child and duplicate all
  // of the member's children.
  auto* relationship = TryGet<Comp::Relationship>(memberId);
  if (relationship != nullptr) {
    if (root && relationship->HasParent()) {
      MakeParent(relationship->mParent, duplicateMemberId);
      relationship = &Get<Comp::Relationship>(memberId);
    }
    for (MemberId childId : relationship->mChildren) {
      MemberId newChildId = Duplicate(childId, false);
      relationship = &Get<Comp::Relationship>(memberId);
      MakeParent(duplicateMemberId, newChildId);
      relationship = &Get<Comp::Relationship>(memberId);
    }
  }
  return duplicateMemberId;
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
  VerifyMemberId(memberId);
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

  for (auto it = mTables.begin(); it != mTables.end(); ++it) {
    Table& table = it->mValue;
    if (table.ValidComponent(memberId)) {
      table.Remove(memberId);
    }
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

void* Space::AddComponent(Comp::TypeId typeId, MemberId owner, bool init)
{
  // Create the component table if necessary and make sure the member doesn't
  // already have the component.
  Table* table = mTables.TryGet(typeId);
  if (table == nullptr) {
    table = &mTables.Emplace(typeId, typeId);
  }
  const Comp::TypeData& typeData = Comp::GetTypeData(typeId);
  if (table->ValidComponent(owner)) {
    std::stringstream error;
    error << "Member " << owner;
    Comp::Name* nameComp = TryGetComponent<Comp::Name>(owner);
    if (nameComp != nullptr) {
      error << " (" << nameComp->mName << ")";
    }
    error << " already has a " << typeData.mName << " (TypeId: " << typeId
          << ") component.";
    LogAbort(error.str().c_str());
  }

  // Add any missing dependencies.
  for (Comp::TypeId dependencyId : typeData.mDependencies) {
    if (!HasComponent(dependencyId, owner)) {
      void* depedency = AddComponent(dependencyId, owner, init);
      const Comp::TypeData& dependencyTypeData =
        Comp::GetTypeData(dependencyId);
      if (dependencyTypeData.mVInit.Open()) {
        const World::Object owner(this, dependencyId);
        dependencyTypeData.mVInit.Invoke(depedency, owner);
      }
    }
  }

  // Create the component.
  void* component = table->Request(owner);
  if (init && typeData.mVInit.Open()) {
    Object ownerObject(this, owner);
    typeData.mVInit.Invoke(component, ownerObject);
  }
  return component;
}

void* Space::EnsureComponent(Comp::TypeId typeId, MemberId owner)
{
  void* component = TryGetComponent(typeId, owner);
  if (component == nullptr) {
    component = AddComponent(typeId, owner);
  }
  return component;
}

void Space::RemComponent(Comp::TypeId typeId, MemberId owner)
{
  VerifyMemberId(owner);
  const Comp::TypeData& typeData = Comp::GetTypeData(typeId);
  Table& table = mTables.Get(typeId);
  if (!table.ValidComponent(owner)) {
    std::stringstream error;
    error << "Member " << owner;
    Comp::Name* nameComp = TryGetComponent<Comp::Name>(owner);
    if (nameComp != nullptr) {
      error << " (" << nameComp->mName << ")";
    }
    error << " does not have a " << typeData.mName << " (TypeId: " << typeId
          << ") component.";
    LogAbort(error.str().c_str());
  }

  // Remove all of the dependant components and the requested component.
  for (Comp::TypeId dependantId : typeData.mDependants) {
    if (HasComponent(dependantId, owner)) {
      RemComponent(dependantId, owner);
    }
  }
  table.Remove(owner);
}

void* Space::GetComponent(Comp::TypeId typeId, MemberId owner) const
{
  VerifyMemberId(owner);
  void* component = TryGetComponent(typeId, owner);
  if (component == nullptr) {
    const Comp::TypeData& typeData = Comp::GetTypeData(typeId);
    std::stringstream error;
    error << "Member " << owner;
    Comp::Name* nameComp = TryGetComponent<Comp::Name>(owner);
    if (nameComp != nullptr) {
      error << " (" << nameComp->mName << ")";
    }
    error << " did not contain a " << typeData.mName << " (" << typeId
          << ") component.";
    LogAbort(error.str().c_str());
  }
  return component;
}

void* Space::TryGetComponent(Comp::TypeId typeId, MemberId owner) const
{
  if (!ValidMemberId(owner)) {
    return nullptr;
  }
  Table* table = mTables.TryGet(typeId);
  if (table == nullptr) {
    return nullptr;
  }
  if (!table->ValidComponent(owner)) {
    return nullptr;
  }
  return table->GetComponent(owner);
}

bool Space::HasComponent(Comp::TypeId typeId, MemberId memberId) const
{
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

Ds::Vector<Comp::TypeId> Space::GetComponentTypes(MemberId owner) const
{
  Ds::Vector<Comp::TypeId> componentTypes;
  for (auto it = mTables.cbegin(); it != mTables.cend(); ++it) {
    const Table& table = it->mValue;
    if (table.ValidComponent(owner)) {
      componentTypes.Push(table.TypeId());
    }
  }
  return componentTypes;
}

const Ds::Map<Comp::TypeId, Table>& Space::Tables() const
{
  return mTables;
}

void Space::Serialize(Vlk::Value& spaceVal) const
{
  for (int i = 0; i < mMembers.DenseUsage(); ++i) {
    // Create the member's value.
    MemberId memberId = mMembers.Dense()[i];
    std::string memberIdStr(std::to_string(memberId));
    Vlk::Value& memberVal = spaceVal(memberIdStr);

    for (auto it = mTables.begin(); it != mTables.end(); ++it) {
      Table& table = it->mValue;
      if (!table.ValidComponent(memberId)) {
        continue;
      }
      const Comp::TypeData& typeData = Comp::nTypeData[table.TypeId()];
      Vlk::Value& componentVal = memberVal(typeData.mName);
      if (!typeData.mVSerialize.Open()) {
        continue;
      }
      void* component = table.GetComponent(memberId);
      typeData.mVSerialize.Invoke(component, componentVal);
    }
  }
}

Result Space::Deserialize(const Vlk::Explorer& spaceEx)
{
  if (!spaceEx.Valid(Vlk::Value::Type::PairArray)) {
    return Result("Space Value must be a ValueArray");
  }

  for (size_t i = 0; i < spaceEx.Size(); ++i) {
    // Create the member.
    Vlk::Explorer memberEx = spaceEx(i);
    std::stringstream idStream(memberEx.Key());
    MemberId memberId;
    idStream >> memberId;
    mMembers.Request(memberId);

    // Get the member's component data.
    World::Object owner(this, memberId);
    for (size_t i = 0; i < memberEx.Size(); ++i) {
      Vlk::Explorer componentEx = memberEx(i);
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
