#ifndef test_world_Print_h
#define test_world_Print_h

#include <iomanip>
#include <iostream>

#include "comp/Type.h"
#include "world/Space.h"
#include "world/Table.h"

void PrintKey()
{
  std::cout << "=Key=\n"
            << "-DescriptorBin-\nRowIndex: [TypeId|TableIndex]...\n\n";
}

// Table Functions /////////////////////////////////////////////////////////////
template<typename T>
void PrintTable(const World::Table& table)
{
  std::cout << "{Owner: [Data]}" << std::endl;
  table.VisitActiveIndices(
    [&table](size_t i)
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
    [&](size_t i)
    {
      std::cout << table.GetOwner(i) << ", ";
    });
  std::cout << "]" << std::endl;
}

void PrintTableStats(const World::Table& table)
{
  size_t stride = table.Stride();
  size_t size = table.Size();
  size_t capacity = table.Capacity();
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
  if (member.DescriptorCount() > 0)
  {
    std::cout << "->";
  }
  space.VisitMemberComponents(
    memberId,
    [](Comp::TypeId typeId, size_t tableIndex)
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

void PrintSpaceDescriptorBin(const World::Space& space)
{
  const Ds::Vector<World::ComponentDescriptor>& descriptorBin =
    space.DescriptorBin();
  const size_t rowSize = 5;
  std::cout << "-DescriptorBin-";
  for (size_t i = 0; i < descriptorBin.Size(); ++i)
  {
    if (i % rowSize == 0)
    {
      std::cout << "\n" << std::setfill('0') << std::setw(2) << i << ": ";
    } else
    {
      std::cout << " ";
    }
    const World::ComponentDescriptor& desc = descriptorBin[i];
    if (!desc.InUse())
    {
      std::cout << "[inv]";
    } else
    {
      std::cout << "[" << desc.mTypeId << "|" << desc.mTableIndex << "]";
    }
  }
  std::cout << "\n";
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
  for (size_t i = 1; i < unusedMemberIds.Size(); ++i)
  {
    std::cout << ", " << unusedMemberIds[i];
  }
  std::cout << std::endl;
}

void PrintSpace(const World::Space& space)
{
  PrintSpaceTables(space);
  PrintSpaceMembers(space);
  PrintSpaceDescriptorBin(space);
}

#endif
