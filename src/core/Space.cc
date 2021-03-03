#include <sstream>

#include "Error.h"

#include "Space.h"

namespace World {

void ComponentAddress::EndUse()
{
  mTable = nInvalidTableRef;
}

bool ComponentAddress::InUse() const
{
  return mTable != nInvalidTableRef;
}

void Member::EndUse()
{
  mAddressIndex = smInvalidAddressIndex;
}

bool Member::InUse() const
{
  return mAddressIndex != smInvalidAddressIndex;
}

int Member::EndAddress() const
{
  return mAddressIndex + mCount;
}

int Member::LastAddress() const
{
  return mAddressIndex + mCount - 1;
}

Space::MemberVisitor::MemberVisitor(Space& space):
  mSpace(space), mCurrentMember(0)
{
  ReachValidMember();
}

Member& Space::MemberVisitor::CurrentMember()
{
  return mSpace.mMembers[mCurrentMember];
}

MemRef Space::MemberVisitor::CurrentMemberRef()
{
  return mCurrentMember;
}

void Space::MemberVisitor::Next()
{
  ++mCurrentMember;
  ReachValidMember();
}

bool Space::MemberVisitor::End()
{
  return mCurrentMember >= mSpace.mMembers.Size();
}

void Space::MemberVisitor::ReachValidMember()
{
  Ds::Vector<Member>& members = mSpace.mMembers;
  while (mCurrentMember < members.Size() && !members[mCurrentMember].InUse())
  {
    ++mCurrentMember;
  }
}

Space::Space(): mName("DefaultName") {}

Space::Space(const std::string& name): mName(name) {}

TableRef Space::RegisterComponentType(int componentId, int size)
{
  TableRef table = mTables.Size();
  Table newTable(size);
  mTables.Push(newTable);
  mTableLookup[componentId] = table;
  return table;
}

Space::MemberVisitor Space::CreateMemberVisitor()
{
  return MemberVisitor(*this);
}

MemRef Space::CreateMember()
{
  // todo: The starting index of the component address used for new members is
  // always the end of the address bin. This ignores unused address memory, and
  // without garbage collection, will result in a perpetually increasing memory
  // overhead. We should be more smart about what address we hand out for a new
  // member in the future.
  int addressIndex = mAddressBin.Size();
  std::stringstream name;

  // If we have unused member references, we use those for the new member before
  // using member references that have yet to be use.
  if (mUnusedMemRefs.Size() > 0)
  {
    MemRef newMemberRef = mUnusedMemRefs.Top();
    Member& newMember = mMembers[newMemberRef];
    newMember.mAddressIndex = addressIndex;
    newMember.mCount = 0;
    name << "Member " << newMemberRef;
    newMember.mName = name.str();
    mUnusedMemRefs.Pop();
    return newMemberRef;
  }
  Member newMember;
  newMember.mAddressIndex = addressIndex;
  newMember.mCount = 0;
  MemRef newMemberRef = mMembers.Size();
  name << "Member " << newMemberRef;
  newMember.mName = name.str();
  mMembers.Push(newMember);
  return newMemberRef;
}

void Space::DeleteMember(MemRef memberRef)
{
  // Verify the existance of the member, remove all of the member's components,
  // and end use of the member memory.
  VerifyMember(memberRef);
  Member& member = mMembers[memberRef];
  int addrIndex = member.mAddressIndex;
  for (; addrIndex < member.EndAddress(); ++addrIndex)
  {
    ComponentAddress& compAddr = mAddressBin[addrIndex];
    mTables[compAddr.mTable].Rem(compAddr.mIndex);
    mAddressBin[addrIndex].EndUse();
  }
  member.EndUse();
  mUnusedMemRefs.Push(memberRef);
}

void* Space::AddComponent(int componentId, MemRef member)
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
  TableRef table = mTableLookup[componentId];
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

void Space::RemComponent(int componentId, MemRef member)
{
  // Verify that the component table and the member both exist.
  VerifyTable(componentId);
  VerifyMember(member);

  // Verify that the member has the component while finding the index of the
  // component address.
  bool foundComponentType = false;
  TableRef table = mTableLookup[componentId];
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

void* Space::GetComponent(int componentId, MemRef member) const
{
  // Make sure the component table and member exist.
  if (!ValidTable(componentId))
  {
    return nullptr;
  }
  VerifyMember(member);

  // Find the requested component by going through the member's component
  // references.
  TableRef table = mTableLookup[componentId];
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

bool Space::HasComponent(int componentId, MemRef member) const
{
  const void* comp = GetComponent(componentId, member);
  return comp != nullptr;
}

const void* Space::GetComponentData(int componentId) const
{
  VerifyTable(componentId);
  TableRef table = mTableLookup[componentId];
  return mTables[table].Data();
}

const Ds::Vector<TableRef>& Space::TableLookup() const
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

const Ds::Vector<MemRef>& Space::UnusedMemRefs() const
{
  return mUnusedMemRefs;
}

const Ds::Vector<ComponentAddress> Space::AddressBin() const
{
  return mAddressBin;
}

bool Space::ValidTable(int componentId) const
{
  // Make sure the table lookup is large enough to treat the component id as an
  // index, the component type has been initialized, and the table lookup has a
  // valid table reference for the component id.
  return componentId < mTableLookup.Size() && componentId >= 0 &&
    mTableLookup[componentId] != nInvalidTableRef;
}

void Space::VerifyTable(int componentId) const
{
  // Make sure that a table exists for the component type.
  LogAbortIf(
    !ValidTable(componentId), "There is no table for this component type.");
}

void Space::VerifyMember(MemRef member) const
{
  // Make sure the member both exists and is valid.
  LogAbortIf(
    member < 0 || member >= mMembers.Size() || !mMembers[member].InUse(),
    "The member under this reference does not exist.");
}

} // namespace World
