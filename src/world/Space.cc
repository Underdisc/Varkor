#include <sstream>

#include "Error.h"

#include "Space.h"

namespace World {

void ComponentAddress::EndUse()
{
  mComponentId = nInvalidComponentId;
}

bool ComponentAddress::InUse() const
{
  return mComponentId != nInvalidComponentId;
}

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

Space::MemberVisitor::MemberVisitor(Space& space):
  mSpace(space), mCurrentMember(0)
{
  ReachValidMember();
}

Member& Space::MemberVisitor::CurrentMember() const
{
  return mSpace.mMembers[mCurrentMember];
}

MemberId Space::MemberVisitor::CurrentMemberId() const
{
  return mCurrentMember;
}

void Space::MemberVisitor::Next()
{
  ++mCurrentMember;
  ReachValidMember();
}

bool Space::MemberVisitor::End() const
{
  return mCurrentMember >= mSpace.mMembers.Size();
}

void Space::MemberVisitor::ReachValidMember()
{
  while (mCurrentMember < mSpace.mMembers.Size())
  {
    Member& member = mSpace.mMembers[mCurrentMember];
    if (member.InUseRootMember())
    {
      break;
    }
    ++mCurrentMember;
  }
}

Space::Space(): mName("DefaultName"), mCameraId(nInvalidMemberId) {}

Space::Space(const std::string& name): mName(name), mCameraId(nInvalidMemberId)
{}

Space::MemberVisitor Space::CreateMemberVisitor()
{
  return MemberVisitor(*this);
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
    name << "Member " << newMemberId;
    newMember.StartUse(addressIndex, name.str());
    mUnusedMemberIds.Pop();
    return newMemberId;
  }
  Member newMember;
  MemberId newMemberId = mMembers.Size();
  name << "Member " << newMemberId;
  newMember.StartUse(addressIndex, name.str());
  mMembers.Push(Util::Move(newMember));
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
    Table* table = mTables.Find(address.mComponentId);
    table->Rem(address.mTableIndex);
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

void* Space::AddComponent(ComponentId componentId, MemberId memberId)
{
  // Create the component table if necessary and make sure the member doesn't
  // already have the component.
  Table* table = mTables.Find(componentId);
  if (table == nullptr)
  {
    ComponentData* data = nComponentTypeData.Find(componentId);
    LogAbortIf(data == nullptr, "This component type has not been registered.");
    Table newTable(data->mSize);
    mTables.Insert(componentId, newTable);
    table = mTables.Find(componentId);
  }
  VerifyMemberId(memberId);
  LogAbortIf(
    HasComponent(componentId, memberId),
    "The member already has this component type.");

  // Create the component, add its address to the address bin, and make that
  // address part of the member.
  ComponentAddress newAddress;
  newAddress.mComponentId = componentId;
  newAddress.mTableIndex = table->Add(memberId);
  Member& member = mMembers[memberId];
  int nextAddressIndex = member.EndAddress();
  if (nextAddressIndex >= mAddressBin.Size())
  {
    mAddressBin.Push(newAddress);
    ++member.mComponentCount;
    return table->GetData(newAddress.mTableIndex);
  }
  const ComponentAddress& compAddr = mAddressBin[nextAddressIndex];
  if (!compAddr.InUse())
  {
    mAddressBin[nextAddressIndex] = newAddress;
    ++member.mComponentCount;
    return table->GetData(newAddress.mTableIndex);
  }
  for (int i = 0; i < member.mComponentCount; ++i)
  {
    int copyAddressIndex = member.mAddressIndex + i;
    ComponentAddress copyAddress = mAddressBin[copyAddressIndex];
    mAddressBin.Push(copyAddress);
    mAddressBin[copyAddressIndex].EndUse();
  }
  mAddressBin.Push(newAddress);
  ++member.mComponentCount;
  member.mAddressIndex = mAddressBin.Size() - member.mComponentCount;
  return table->GetData(newAddress.mTableIndex);
}

void Space::RemComponent(ComponentId componentId, MemberId memberId)
{
  // Find the address index for the member's component.
  VerifyMemberId(memberId);
  Member& member = mMembers[memberId];
  int addressIndex = member.mAddressIndex;
  int endIndex = member.EndAddress();
  while (addressIndex < endIndex)
  {
    if (mAddressBin[addressIndex].mComponentId == componentId)
    {
      break;
    }
    ++addressIndex;
  }
  LogAbortIf(
    addressIndex == endIndex, "The member does not have the component type.");

  // Remove the old component address from the member's address list.
  ComponentAddress& address = mAddressBin[addressIndex];
  Table* table = GetTable(componentId);
  table->Rem(address.mTableIndex);
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

void* Space::GetComponent(ComponentId componentId, MemberId memberId) const
{
  if (!ValidMemberId(memberId))
  {
    return nullptr;
  }
  int tableIndex = GetTableIndex(componentId, memberId);
  if (tableIndex == Table::smInvalidIndex)
  {
    return nullptr;
  }
  Table* table = GetTable(componentId);
  return table->GetData(tableIndex);
}

bool Space::HasComponent(ComponentId componentId, MemberId memberId) const
{
  if (!ValidMemberId(memberId))
  {
    return false;
  }
  int tableIndex = GetTableIndex(componentId, memberId);
  return tableIndex != Table::smInvalidIndex;
}

const void* Space::GetComponentData(ComponentId component) const
{
  return GetTable(component)->Data();
}

const Ds::Map<ComponentId, Table>& Space::Tables() const
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

Table* Space::GetTable(ComponentId component) const
{
  Table* table = mTables.Find(component);
  LogAbortIf(table == nullptr, "There is no table for this component type.");
  return table;
}

int Space::GetTableIndex(ComponentId componentId, MemberId memberId) const
{
  const Member& member = mMembers[memberId];
  for (int i = 0; i < member.mComponentCount; ++i)
  {
    const ComponentAddress& address = mAddressBin[member.mAddressIndex + i];
    if (address.mComponentId == componentId)
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
