#include <iostream>

#include "Print.h"

void PrintTableStats(const World::Table& table)
{
  int stride = table.Stride();
  int size = table.Size();
  int capacity = table.Capacity();
  std::cout << "Stride: " << stride << std::endl
            << "Size: " << size << std::endl
            << "SizeInBytes: " << size * stride << std::endl
            << "Capacity: " << capacity << std::endl
            << "CapacityInBytes: " << capacity * stride << std::endl;
}

void PrintTableOwners(const World::Table& table)
{
  std::cout << "Owners: ";
  int size = table.Size();
  if (size == 0)
  {
    std::cout << "[]" << std::endl;
    return;
  }
  std::cout << "[" << table.GetOwner(0);
  for (int i = 1; i < size; ++i)
  {
    std::cout << ", " << table.GetOwner(i);
  }
  std::cout << "]" << std::endl;
}

void PrintSpace(const World::Space& space)
{
  PrintSpaceTables(space);
  PrintSpaceTableLookup(space);
  PrintSpaceMembers(space);
  PrintSpaceAddressBin(space);
}

void PrintSpaceTables(const World::Space& space)
{
  const Ds::Vector<World::Table>& tables = space.Tables();
  for (int i = 0; i < tables.Size(); ++i)
  {
    std::cout << i << " {" << std::endl;
    PrintTableStats(tables[i]);
    PrintTableOwners(tables[i]);
    std::cout << "}" << std::endl;
  }
}

void PrintSpaceTableLookup(const World::Space& space)
{
  const Ds::Vector<World::TableId>& lookup = space.TableLookup();
  std::cout << "TableLookup: [ComponentId, Table]";
  for (int i = 0; i < lookup.Size(); ++i)
  {
    std::cout << ", [" << i << ", " << lookup[i] << "]";
  }
  std::cout << std::endl;
}

void PrintSpaceTablesOwners(const World::Space& space)
{
  const Ds::Vector<World::Table>& tables = space.Tables();
  for (int i = 0; i < tables.Size(); ++i)
  {
    std::cout << "Table " << i << " ";
    PrintTableOwners(tables[i]);
  }
}

void PrintSpaceMember(
  const World::Space& space,
  World::MemberId memberId,
  const std::string& indent = "")
{
  const World::Member& member = space.Members()[memberId];
  std::cout << indent << "[" << memberId << ", " << member.AddressIndex()
            << ", " << member.Count() << "]" << std::endl;
  for (World::MemberId childId : member.Children())
  {
    PrintSpaceMember(space, childId, indent + "| ");
  }
}

void PrintSpaceMembers(const World::Space& space)
{
  std::cout << "-[MemberId, Address, Count]-" << std::endl;
  const Ds::Vector<World::Member>& members = space.Members();
  for (World::MemberId memberId = 0; memberId < members.Size(); ++memberId)
  {
    const World::Member& member = members[memberId];
    if (member.InUseRootMember())
    {
      PrintSpaceMember(space, memberId);
    }
  }
}

void PrintSpaceAddressBin(const World::Space& space)
{
  const Ds::Vector<World::ComponentAddress>& addressBin = space.AddressBin();
  std::cout << "AddressBin: [Table, Index]";
  for (const World::ComponentAddress& addr : addressBin)
  {
    std::cout << ", ";
    if (!addr.InUse())
    {
      std::cout << "[inv]";
      continue;
    }
    std::cout << "[" << addr.mTable << ", " << addr.mIndex << "]";
  }
  std::cout << std::endl;
}

void PrintSpaceUnusedMemberIds(const World::Space& space)
{
  const Ds::Vector<World::MemberId>& unusedMemberIds = space.UnusedMemberIds();
  std::cout << "UnusedMemberIds: ";
  if (unusedMemberIds.Size() == 0)
  {
    std::cout << "None" << std::endl;
    return;
  }
  std::cout << unusedMemberIds[0];
  for (int i = 1; i < unusedMemberIds.Size(); ++i)
  {
    std::cout << ", " << unusedMemberIds[i];
  }
  std::cout << std::endl;
}
