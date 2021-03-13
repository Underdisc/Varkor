#include <sstream>

#include "Error.h"

#include "Space.h"

namespace World {

void ComponentAddress::EndUse()
{
  mTable = nInvalidTableId;
}

bool ComponentAddress::InUse() const
{
  return mTable != nInvalidTableId;
}

void Member::StartUse(int addressIndex, const std::string& name)
{
  mAddressIndex = addressIndex;
  mCount = 0;
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
  return mAddressIndex + mCount;
}

int Member::LastAddress() const
{
  return mAddressIndex + mCount - 1;
}

int Member::AddressIndex() const
{
  return mAddressIndex;
}

ObjSizeT Member::Count() const
{
  return mCount;
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
  }
}

Space::Space(): mName("DefaultName") {}

Space::Space(const std::string& name): mName(name) {}

TableId Space::RegisterComponentType(int componentId, int size)
{
  TableId table = mTables.Size();
  Table newTable(size);
  mTables.Push(newTable);
  mTableLookup[componentId] = table;
  return table;
}

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

void Space::DeleteMember(MemberId memberId)
{
  // Verify the existance of the member.
  VerifyMember(memberId);
  Member& member = mMembers[memberId];

  // Delete all of the children of the member to be deleted.
  for (MemberId childId : member.mChildren)
  {
    DeleteMember(childId);
  }

  // Remove all of the member's components and end use of the member's memory.
  int addrIndex = member.mAddressIndex;
  for (; addrIndex < member.EndAddress(); ++addrIndex)
  {
    ComponentAddress& compAddr = mAddressBin[addrIndex];
    mTables[compAddr.mTable].Rem(compAddr.mIndex);
    mAddressBin[addrIndex].EndUse();
  }
  member.EndUse();
  mUnusedMemberIds.Push(memberId);
}

void Space::MakeParent(MemberId parentId, MemberId childId)
{
  // Verify the existance of the parent and child members.
  VerifyMember(parentId);
  VerifyMember(childId);
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
  VerifyMember(childId);
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
  VerifyMember(id);
  return mMembers[id];
}

void* Space::AddComponent(int componentId, MemberId member)
{
  // Verify that the component table exists, the member exist, and the member
  // doesn't already have the component being added.
  VerifyTable(componentId);
  VerifyMember(member);
  LogAbortIf(
    HasComponent(componentId, member),
    "This member already has this type of component.");

  // Allocate data for the new component and save a pointer to the component
  // data so it can be used as the return value.
  TableId table = mTableLookup[componentId];
  ComponentAddress newAddress;
  newAddress.mTable = table;
  newAddress.mIndex = mTables[table].Add(member);
  void* componentData = mTables[table].GetData(newAddress.mIndex);

  // Add the component's address to the address bin, and make that address part
  // of the member.
  Member& selected = mMembers[member];
  int nextAddressIndex = selected.EndAddress();
  if (nextAddressIndex >= mAddressBin.Size())
  {
    mAddressBin.Push(newAddress);
    ++selected.mCount;
    return componentData;
  }
  const ComponentAddress& compAddr = mAddressBin[nextAddressIndex];
  if (!compAddr.InUse())
  {
    mAddressBin[nextAddressIndex] = newAddress;
    ++selected.mCount;
    return componentData;
  }
  for (int i = 0; i < selected.mCount; ++i)
  {
    int copyAddrIndex = selected.mAddressIndex + i;
    ComponentAddress copyAddr = mAddressBin[copyAddrIndex];
    mAddressBin.Push(copyAddr);
    mAddressBin[copyAddrIndex].EndUse();
  }
  mAddressBin.Push(newAddress);
  ++selected.mCount;
  selected.mAddressIndex = mAddressBin.Size() - selected.mCount;
  return componentData;
}

void Space::RemComponent(int componentId, MemberId member)
{
  // Verify that the component table and the member both exist.
  VerifyTable(componentId);
  VerifyMember(member);

  // Verify that the member has the component while finding the index of the
  // component address.
  bool foundComponentType = false;
  TableId table = mTableLookup[componentId];
  Member& selected = mMembers[member];
  int compAddrIndex = selected.mAddressIndex;
  int endAddr = selected.EndAddress();
  for (; compAddrIndex < endAddr; ++compAddrIndex)
  {
    if (mAddressBin[compAddrIndex].mTable == table)
    {
      foundComponentType = true;
      break;
    }
  }
  LogAbortIf(
    !foundComponentType,
    "The member did not have the component type to be removed.");

  // Remove the old component address from the member's address list.
  ComponentAddress& compAddr = mAddressBin[compAddrIndex];
  mTables[table].Rem(compAddr.mIndex);
  if (compAddrIndex == selected.LastAddress())
  {
    compAddr.EndUse();
  } else
  {
    ComponentAddress& lastCompAddr = mAddressBin[selected.LastAddress()];
    compAddr = lastCompAddr;
    lastCompAddr.EndUse();
  }
  --selected.mCount;
}

void* Space::GetComponent(int componentId, MemberId member) const
{
  // Make sure the component table and member exist.
  if (!ValidTable(componentId))
  {
    return nullptr;
  }
  VerifyMember(member);

  // Find the requested component by going through the member's component
  // references.
  TableId table = mTableLookup[componentId];
  const Member& selected = mMembers[member];
  for (int i = 0; i < selected.mCount; ++i)
  {
    const ComponentAddress& address = mAddressBin[selected.mAddressIndex + i];
    if (address.mTable == table)
    {
      return mTables[address.mTable].GetData(address.mIndex);
    }
  }
  return nullptr;
}

bool Space::HasComponent(int componentId, MemberId member) const
{
  const void* comp = GetComponent(componentId, member);
  return comp != nullptr;
}

const void* Space::GetComponentData(int componentId) const
{
  VerifyTable(componentId);
  TableId table = mTableLookup[componentId];
  return mTables[table].Data();
}

const Ds::Vector<TableId>& Space::TableLookup() const
{
  return mTableLookup;
}

const Ds::Vector<Table>& Space::Tables() const
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

bool Space::ValidTable(int componentId) const
{
  // Make sure the table lookup is large enough to treat the component id as an
  // index, the component type has been initialized, and the table lookup has a
  // valid table id for the component id.
  return componentId < mTableLookup.Size() && componentId >= 0 &&
    mTableLookup[componentId] != nInvalidTableId;
}

void Space::VerifyTable(int componentId) const
{
  // Make sure that a table exists for the component type.
  LogAbortIf(
    !ValidTable(componentId), "There is no table for this component type.");
}

void Space::VerifyMember(MemberId member) const
{
  // Make sure the member both exists and is valid.
  LogAbortIf(
    member < 0 || member >= mMembers.Size() || !mMembers[member].InUse(),
    "The member under this reference does not exist.");
}

} // namespace World
