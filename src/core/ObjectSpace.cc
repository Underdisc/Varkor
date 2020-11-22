#include <iostream>

#include "Error.h"

#include "ObjectSpace.h"

namespace Core {

void ComponentAddress::EndUse()
{
  mTable = nInvalidTableRef;
}

bool ComponentAddress::InUse() const
{
  return mTable != nInvalidTableRef;
}

std::ostream& operator<<(std::ostream& os, const ComponentAddress& addr)
{
  if (!addr.InUse())
  {
    os << "[inv]";
    return os;
  }
  os << "[" << addr.mTable << ", " << addr.mIndex << "]";
  return os;
}

int Member::smInvalidMemberAddress = -1;

int Member::EndAddress() const
{
  return mAddressIndex + mCount;
}

int Member::LastAddress() const
{
  return mAddressIndex + mCount - 1;
}

bool Member::Valid() const
{
  return mAddressIndex != smInvalidMemberAddress;
}

TableRef Space::RegisterComponentType(int componentId, int size)
{
  TableRef table = mTables.Size();
  ComponentTable newTable(size);
  mTables.Push(newTable);
  mTableLookup[componentId] = table;
  return table;
}

MemRef Space::CreateMember()
{
  Member newMember;
  newMember.mAddressIndex = mAddressBin.Size();
  newMember.mCount = 0;
  MemRef newMemberRef = mMembers.Size();
  mMembers.Push(newMember);
  return newMemberRef;
}

void* Space::AddComponent(int componentId, MemRef member)
{
  // Verify that the component table exists, the member exist, and the member
  // doesn't already have the component being added.
  VerifyComponentTable(componentId);
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
  void* componentData = mTables[table][newAddress.mIndex];

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
  VerifyComponentTable(componentId);
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
  VerifyComponentTable(componentId);
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
      return mTables[address.mTable][address.mIndex];
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
  VerifyComponentTable(componentId);
  TableRef table = mTableLookup[componentId];
  return mTables[table].Data();
}

void Space::ShowAll() const
{
  ShowTables();
  ShowTableLookup();
  ShowMembers();
  ShowAddressBin();
}

void Space::ShowTableLookup() const
{
  std::cout << "TableLookup: [ComponentId, Table]";
  for (int i = 0; i < mTableLookup.Size(); ++i)
  {
    std::cout << ", [" << i << ", " << mTableLookup[i] << "]";
  }
  std::cout << std::endl;
}

void Space::ShowTable(int componentId) const
{
  VerifyComponentTable(componentId);
  TableRef table = mTableLookup[componentId];
  mTables[table].ShowStats();
  mTables[table].ShowOwners();
}

void Space::ShowTables() const
{
  for (int i = 0; i < mTables.Size(); ++i)
  {
    std::cout << i << " {" << std::endl;
    mTables[i].ShowStats();
    mTables[i].ShowOwners();
    std::cout << "}" << std::endl;
  }
}

void Space::ShowMembers() const
{
  std::cout << "Members: [Address, Count]";
  for (const Member& member : mMembers)
  {
    std::cout << ", [" << member.mAddressIndex << ", " << member.mCount << "]";
  }
  std::cout << std::endl;
}

void Space::ShowAddressBin() const
{
  std::cout << "AddressBin: [Table, Index]";
  for (int i = 0; i < mAddressBin.Size(); ++i)
  {
    std::cout << ", " << mAddressBin[i];
  }
  std::cout << std::endl;
}

bool Space::ValidComponentTable(int componentId) const
{
  // Make sure the table lookup is large enough to treat the component id as an
  // index, the component type has been initialized, and the table lookup has a
  // valid table reference for the component id.
  return componentId < mTableLookup.Size() && componentId >= 0 &&
    mTableLookup[componentId] != nInvalidTableRef;
}

void Space::VerifyComponentTable(int componentId) const
{
  // Make sure that a table exists for the component type.
  LogAbortIf(
    !ValidComponentTable(componentId),
    "There is no table for this component type.");
}

void Space::VerifyMember(MemRef member) const
{
  // Make sure the member both exists and is valid.
  LogAbortIf(
    member < 0 || member >= mMembers.Size() || !mMembers[member].Valid(),
    "The member under this reference does not exist.");
}

} // namespace Core
