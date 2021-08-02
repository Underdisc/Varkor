#include <sstream>

#include "Error.h"
#include "vlk/Explorer.h"
#include "vlk/Pair.h"
#include "world/Space.h"

namespace World {

void ComponentAddress::EndUse()
{
  mTypeId = Comp::nInvalidTypeId;
}

bool ComponentAddress::InUse() const
{
  return mTypeId != Comp::nInvalidTypeId;
}

Member::Member(): mAddressIndex(smInvalidAddressIndex) {}

void Member::StartUse(int addressIndex, const std::string& name)
{
  mAddressIndex = addressIndex;
  mComponentCount = 0;
  mParent = nInvalidMemberId;
  mName = name;
}

void Member::EndUse()
{
  mAddressIndex = smInvalidAddressIndex;
  mChildren.Clear();
}

bool Member::InUse() const
{
  return mAddressIndex != smInvalidAddressIndex;
}

bool Member::HasParent() const
{
  return mParent != nInvalidMemberId;
}

bool Member::InUseRootMember() const
{
  return InUse() && !HasParent();
}

int Member::EndAddress() const
{
  return mAddressIndex + mComponentCount;
}

int Member::LastAddress() const
{
  return mAddressIndex + mComponentCount - 1;
}

int Member::AddressIndex() const
{
  return mAddressIndex;
}

int Member::ComponentCount() const
{
  return mComponentCount;
}

MemberId Member::Parent() const
{
  return mParent;
}

const Ds::Vector<MemberId>& Member::Children() const
{
  return mChildren;
}

Space::Space(): mName("DefaultName"), mCameraId(nInvalidMemberId) {}

Space::Space(const std::string& name): mName(name), mCameraId(nInvalidMemberId)
{}

void Space::Update()
{
  for (const Ds::KvPair<Comp::TypeId, Table>& tablePair : mTables)
  {
    tablePair.mValue.UpdateComponents();
  }
}

MemberId Space::CreateMember()
{
  // todo: The starting index of the component address used for new members is
  // always the end of the address bin. This ignores unused address memory, and
  // without garbage collection, will result in a perpetually increasing memory
  // overhead. We should be more smart about what address we hand out for a new
  // member in the future.
  int addressIndex = mAddressBin.Size();
  std::stringstream name;

  // If we have unused member ids, we use those for the new member before using
  // member ids that have yet to be used.
  if (mUnusedMemberIds.Size() > 0)
  {
    MemberId newMemberId = mUnusedMemberIds.Top();
    Member& newMember = mMembers[newMemberId];
    name << "Member" << newMemberId;
    newMember.StartUse(addressIndex, name.str());
    mUnusedMemberIds.Pop();
    return newMemberId;
  }
  Member newMember;
  MemberId newMemberId = mMembers.Size();
  name << "Member" << newMemberId;
  newMember.StartUse(addressIndex, name.str());
  mMembers.Push(Util::Move(newMember));
  return newMemberId;
}

MemberId Space::Duplicate(MemberId ogMemberId, bool duplicationRoot)
{
  // Duplicate all of the components from the original member.
  VerifyMemberId(ogMemberId);
  MemberId newMemberId = CreateMember();
  const Member& ogMember = mMembers[ogMemberId];
  int addressIndex = ogMember.mAddressIndex;
  while (addressIndex < ogMember.EndAddress())
  {
    ComponentAddress newAddress;
    const ComponentAddress& address = mAddressBin[addressIndex];
    newAddress.mTypeId = address.mTypeId;
    Table& table = mTables.Get(address.mTypeId);
    newAddress.mTableIndex = table.Duplicate(address.mTableIndex, newMemberId);
    AddAddressToMember(newMemberId, newAddress);
    ++addressIndex;
  }

  // Make the new member a child if we are duplicating a child and duplicate all
  // of the member's children.
  if (duplicationRoot && ogMember.HasParent())
  {
    MakeParent(ogMember.Parent(), newMemberId);
  }
  for (MemberId childId : ogMember.mChildren)
  {
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
  for (MemberId childId : member.mChildren)
  {
    DeleteMember(childId);
  }

  // Remove all of the member's components and end its use.
  int addrIndex = member.mAddressIndex;
  for (; addrIndex < member.EndAddress(); ++addrIndex)
  {
    ComponentAddress& address = mAddressBin[addrIndex];
    Table& table = mTables.Get(address.mTypeId);
    table.Rem(address.mTableIndex);
    mAddressBin[addrIndex].EndUse();
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
  if (childMember.HasParent())
  {
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
  Member& childParent = mMembers[childMember.mParent];
  for (int i = 0; i < childParent.mChildren.Size(); ++i)
  {
    if (childParent.mChildren[i] == childId)
    {
      childParent.mChildren.LazyRemove(i);
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

void* Space::AddComponent(
  Comp::TypeId typeId, MemberId memberId, bool construct)
{
  // Create the component table if necessary and make sure the member doesn't
  // already have the component.
  Table* table = mTables.Find(typeId);
  if (table == nullptr)
  {
    Table newTable(typeId);
    mTables.Insert(typeId, newTable);
    table = mTables.Find(typeId);
  }
  VerifyMemberId(memberId);
  LogAbortIf(
    HasComponent(typeId, memberId),
    "The member already has this component type.");

  // Create the component and make it part of the member
  ComponentAddress newAddress;
  newAddress.mTypeId = typeId;
  if (construct)
  {
    newAddress.mTableIndex = table->Add(memberId);
  } else
  {
    newAddress.mTableIndex = table->AllocateComponent(memberId);
  }
  AddAddressToMember(memberId, newAddress);
  return table->GetComponent(newAddress.mTableIndex);
}

void Space::AddAddressToMember(
  MemberId memberId, const ComponentAddress& address)
{
  Member& member = mMembers[memberId];
  int nextAddressIndex = member.EndAddress();
  if (nextAddressIndex >= mAddressBin.Size())
  {
    mAddressBin.Push(address);
    ++member.mComponentCount;
    return;
  }
  const ComponentAddress& compAddr = mAddressBin[nextAddressIndex];
  if (!compAddr.InUse())
  {
    mAddressBin[nextAddressIndex] = address;
    ++member.mComponentCount;
    return;
  }
  for (int i = 0; i < member.mComponentCount; ++i)
  {
    int copyAddressIndex = member.mAddressIndex + i;
    ComponentAddress copyAddress = mAddressBin[copyAddressIndex];
    mAddressBin.Push(copyAddress);
    mAddressBin[copyAddressIndex].EndUse();
  }
  mAddressBin.Push(address);
  ++member.mComponentCount;
  member.mAddressIndex = mAddressBin.Size() - member.mComponentCount;
}

void Space::RemComponent(Comp::TypeId typeId, MemberId memberId)
{
  // Find the address index for the member's component.
  VerifyMemberId(memberId);
  Member& member = mMembers[memberId];
  int addressIndex = member.mAddressIndex;
  int endIndex = member.EndAddress();
  while (addressIndex < endIndex)
  {
    if (mAddressBin[addressIndex].mTypeId == typeId)
    {
      break;
    }
    ++addressIndex;
  }
  LogAbortIf(
    addressIndex == endIndex, "The member does not have the component type.");

  // Remove the old component address from the member's address list.
  ComponentAddress& address = mAddressBin[addressIndex];
  Table& table = mTables.Get(typeId);
  table.Rem(address.mTableIndex);
  if (addressIndex == member.LastAddress())
  {
    address.EndUse();
  } else
  {
    ComponentAddress& lastAddress = mAddressBin[member.LastAddress()];
    address = lastAddress;
    lastAddress.EndUse();
  }
  --member.mComponentCount;
}

void* Space::GetComponent(Comp::TypeId typeId, MemberId memberId) const
{
  if (!ValidMemberId(memberId))
  {
    return nullptr;
  }
  int tableIndex = GetTableIndex(typeId, memberId);
  if (tableIndex == Table::smInvalidIndex)
  {
    return nullptr;
  }
  Table& table = mTables.Get(typeId);
  return table.GetComponent(tableIndex);
}

bool Space::HasComponent(Comp::TypeId typeId, MemberId memberId) const
{
  if (!ValidMemberId(memberId))
  {
    return false;
  }
  int tableIndex = GetTableIndex(typeId, memberId);
  return tableIndex != Table::smInvalidIndex;
}

void Space::VisitRootMemberIds(std::function<void(World::MemberId)> visit) const
{
  for (MemberId i = 0; i < mMembers.Size(); ++i)
  {
    const Member& member = mMembers[i];
    if (member.InUseRootMember())
    {
      visit(i);
    }
  }
}

void Space::VisitMemberComponents(
  MemberId memberId, std::function<void(Comp::TypeId, int)> visit) const
{
  VerifyMemberId(memberId);
  const Member& member = mMembers[memberId];
  int currentAddressIndex = member.mAddressIndex;
  while (currentAddressIndex < member.EndAddress())
  {
    const ComponentAddress& address = mAddressBin[currentAddressIndex];
    visit(address.mTypeId, address.mTableIndex);
    ++currentAddressIndex;
  }
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

const Ds::Vector<ComponentAddress> Space::AddressBin() const
{
  return mAddressBin;
}

void Space::Serialize(Vlk::Pair& spaceVlk) const
{
  spaceVlk("CameraId") = mCameraId;
  Vlk::Pair& membersVlk = spaceVlk("Members");
  for (MemberId memberId = 0; memberId < mMembers.Size(); ++memberId)
  {
    // We only handle members that are in use.
    const Member& member = mMembers[memberId];
    if (!member.InUse())
    {
      continue;
    }

    // Serialize all of the member's data.
    Vlk::Pair& memberVlk = membersVlk(member.mName.c_str());
    memberVlk("Id") = memberId;
    memberVlk("Parent") = member.mParent;
    Vlk::Value& childrenVlk = memberVlk("Children")[{member.mChildren.Size()}];
    for (int i = 0; i < member.mChildren.Size(); ++i)
    {
      childrenVlk[i] = member.mChildren[i];
    }
    Vlk::Pair& componentsVlk = memberVlk("Components");
    VisitMemberComponents(
      memberId,
      [&](Comp::TypeId typeId, int tableIndex)
      {
        Comp::TypeData& typeData = Comp::nTypeData[typeId];
        Vlk::Pair& componentVlk = componentsVlk(typeData.mName);
        Table& table = mTables.Get(typeId);
        typeData.mVSerialize.Invoke(table[tableIndex], componentVlk);
      });
  }
}

void Space::Deserialize(const Vlk::Explorer& spaceEx)
{
  mName = spaceEx.Key();
  mCameraId = spaceEx("CameraId").As<int>(nInvalidMemberId);
  Vlk::Explorer membersEx = spaceEx("Members");
  if (!membersEx.Valid())
  {
    LogError("Spaces should have a list of Members.");
    return;
  }
  for (int i = 0; i < membersEx.Size(); ++i)
  {
    // Ensure that the current member is valid.
    Vlk::Explorer memberEx = membersEx(i);
    MemberId memberId = memberEx("Id").As<int>(nInvalidMemberId);
    if (memberId == nInvalidMemberId)
    {
      LogError("A Member should have a valid Id.");
      continue;
    }

    // Deserialize the member's data.
    if (memberId >= mMembers.Size())
    {
      mMembers.Resize(memberId + 1);
    }
    Member& member = mMembers[memberId];
    member.StartUse(mAddressBin.Size(), memberEx.Key());
    member.mParent = memberEx("Parent").As<int>(nInvalidMemberId);
    Vlk::Explorer childrenEx = memberEx("Children");
    for (int i = 0; i < childrenEx.Size(); ++i)
    {
      member.mChildren.Push(childrenEx[i].As<int>());
    }
    Vlk::Explorer componentsEx = memberEx("Components");
    for (int i = 0; i < componentsEx.Size(); ++i)
    {
      Vlk::Explorer componentEx = componentsEx(i);
      Comp::TypeId typeId = Comp::GetTypeId(componentEx.Key());
      const Comp::TypeData& typeData = Comp::GetTypeData(typeId);
      void* component = AddComponent(typeId, memberId, false);
      typeData.mVDeserialize.Invoke(component, componentEx);
    }
  }
}

int Space::GetTableIndex(Comp::TypeId typeId, MemberId memberId) const
{
  const Member& member = mMembers[memberId];
  for (int i = 0; i < member.mComponentCount; ++i)
  {
    const ComponentAddress& address = mAddressBin[member.mAddressIndex + i];
    if (address.mTypeId == typeId)
    {
      return address.mTableIndex;
    }
  }
  return Table::smInvalidIndex;
}

bool Space::ValidMemberId(MemberId memberId) const
{
  return memberId >= 0 && memberId < mMembers.Size() &&
    mMembers[memberId].InUse();
}

void Space::VerifyMemberId(MemberId memberId) const
{
  LogAbortIf(!ValidMemberId(memberId), "This member does not exist.");
}

} // namespace World
