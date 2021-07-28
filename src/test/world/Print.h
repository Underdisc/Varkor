#ifndef test_world_Print_h
#define test_world_Print_h

#include <iostream>

#include "comp/Type.h"
#include "world/Space.h"
#include "world/Table.h"

// Table Functions /////////////////////////////////////////////////////////////
template<typename T>
void PrintTable(const World::Table& table)
{
  std::cout << "{Owner: [Data]}" << std::endl;
  table.VisitActiveIndices(
    [&table](int i)
    {
      std::cout << "{" << table.GetOwner(i) << ": ";
      (*(T*)table[i]).PrintData();
      std::cout << "}" << std::endl;
    });
}
template<typename T>
void PrintTableData(const World::Table& table)
{
  table.VisitComponents(
    [](void* component)
    {
      (*(T*)component).PrintData();
    });
  std::cout << std::endl;
}

void PrintTableOwners(const World::Table& table)
{
  std::cout << "Owners: [";
  table.VisitActiveIndices(
    [&](int i)
    {
      std::cout << table.GetOwner(i) << ", ";
    });
  std::cout << "]" << std::endl;
}

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

// Space Functions /////////////////////////////////////////////////////////////
void PrintSpaceTables(const World::Space& space)
{
  for (const auto& tablePair : space.Tables())
  {
    std::cout << tablePair.Key() << " {" << std::endl;
    PrintTableStats(tablePair.mValue);
    PrintTableOwners(tablePair.mValue);
    std::cout << "}" << std::endl;
  }
}

void PrintSpaceTablesOwners(const World::Space& space)
{
  for (const auto& tablePair : space.Tables())
  {
    std::cout << "Table " << tablePair.Key() << " ";
    PrintTableOwners(tablePair.mValue);
  }
}

void PrintSpaceMember(
  const World::Space& space,
  World::MemberId memberId,
  const std::string& indent = "")
{
  const World::Member& member = space.Members()[memberId];
  std::cout << indent << "{" << memberId << "}";
  if (member.ComponentCount() > 0)
  {
    std::cout << "->";
  }
  space.VisitMemberComponents(
    memberId,
    [](Comp::TypeId typeId, int tableIndex)
    {
      std::cout << "[" << typeId << ", " << tableIndex << "]";
    });
  std::cout << std::endl;
  for (World::MemberId childId : member.Children())
  {
    PrintSpaceMember(space, childId, indent + "| ");
  }
}

void PrintSpaceMembers(const World::Space& space)
{
  std::cout << "-Members {MemberId}->[TypeId, TableIndex]...-" << std::endl;
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
  for (const World::ComponentAddress& address : addressBin)
  {
    std::cout << ", ";
    if (!address.InUse())
    {
      std::cout << "[inv]";
      continue;
    }
    std::cout << "[" << address.mTypeId << ", " << address.mTableIndex << "]";
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

void PrintSpace(const World::Space& space)
{
  PrintSpaceTables(space);
  PrintSpaceMembers(space);
  PrintSpaceAddressBin(space);
}

#endif
