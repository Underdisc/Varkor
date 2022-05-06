#ifndef test_world_Print_h
#define test_world_Print_h

#include <iomanip>
#include <iostream>

#include "comp/Type.h"
#include "test/world/TestTypes.h"
#include "world/Space.h"
#include "world/Table.h"

template<typename T>
void PrintRegistration()
{
  const Comp::TypeData& typeData = Comp::GetTypeData<T>();
  std::cout << "-" << Comp::Type<T>::smId << "-"
            << "\nName: " << typeData.mName << "\nSize: " << typeData.mSize
            << "\nDependencies: [";
  for (size_t i = 0; i < typeData.mDependencies.Size(); ++i) {
    std::cout << typeData.mDependencies[i];
    if (i < typeData.mDependencies.Size() - 1) {
      std::cout << ", ";
    }
  }
  std::cout << "]\nDependants: [";
  for (size_t i = 0; i < typeData.mDependants.Size(); ++i) {
    std::cout << typeData.mDependants[i];
    if (i < typeData.mDependants.Size() - 1) {
      std::cout << ", ";
    }
  }
  std::cout << "]\n";
}

void PrintTableStats(const World::Table& table)
{
  std::cout << "-TableStats-\n"
            << "Stride: " << table.Stride() << ", Size: " << table.Size()
            << ", Capacity: " << table.Capacity() << "\n";
}

void PrintTableOwners(const World::Table& table)
{
  std::cout << "-TableOwners-\n[";
  for (size_t i = 0; i < table.Size(); ++i) {
    std::cout << table.GetOwner(i);
    if (i < table.Size() - 1) {
      std::cout << ", ";
    }
  }
  std::cout << "]\n";
}

template<typename T>
void PrintTable(const World::Table& table)
{
  std::cout << "-TableData-\n";
  for (size_t i = 0; i < table.Size(); ++i) {
    World::MemberId owner = table.GetOwner(i);
    if (owner == World::nInvalidMemberId) {
      continue;
    }
    std::cout << table.GetOwner(i) << ": ";
    T* component = (T*)table.GetComponent(i);
    component->PrintData();
    std::cout << "\n";
  }
}

void PrintKey()
{
  std::cout << "=Key=\n"
            << "-ComponentData-\nTypeId: [Data]...\n"
            << "-DescriptorBin-\nRowIndex: [TypeId|TableIndex]...\n"
            << "-MemberBin-\nRowMemberId: [FirstDescriptor|LastDescriptor]...\n"
            << "-Relationships-\nMemberId: [ChildId|ChildParentId]\n"
            << "-TableOwners-\nTypeId: [Owner, ...]\n"
            << "-TableStats-\nTypeId: Stats, ...\n\n";
}

template<typename T>
void PrintSpaceComponentData(const World::Space& space)
{
  std::cout << Comp::Type<T>::smId << ": ";
  Ds::Vector<World::MemberId> slice = space.Slice<T>();
  for (int i = 0; i < slice.Size(); ++i) {
    space.Get<T>(slice[i]).PrintData();
  }
  std::cout << std::endl;
}

void PrintSpaceTestTypeComponentData(const World::Space& space)
{
  std::cout << "-ComponentData-\n";
  PrintSpaceComponentData<Simple0>(space);
  PrintSpaceComponentData<Simple1>(space);
  PrintSpaceComponentData<Dynamic>(space);
  PrintSpaceComponentData<Container>(space);
}

void PrintSpaceTablesStats(const World::Space& space)
{
  std::cout << "-TableStats-\n";
  for (const auto& tablePair : space.Tables()) {
    const World::Table& table = tablePair.mValue;
    std::cout << tablePair.Key() << ": Stride: " << table.Stride()
              << ", Size: " << table.Size()
              << ", Capacity: " << table.Capacity() << "\n";
  }
}

void PrintSpaceTablesOwners(const World::Space& space)
{
  std::cout << "-TableOwners-\n";
  for (const auto& tablePair : space.Tables()) {
    std::cout << tablePair.Key() << ": [";
    const World::Table& table = tablePair.mValue;
    for (size_t i = 0; i < table.Size(); ++i) {
      std::cout << table.GetOwner(i);
      if (i < table.Size() - 1) {
        std::cout << ", ";
      }
    }
    std::cout << "]\n";
  }
}

void PrintSpaceRelationships(const World::Space& space)
{
  std::cout << "-Relationships-";
  bool noRelationships = true;
  const Ds::Vector<World::Member>& members = space.Members();
  for (World::MemberId memberId = 0; memberId < members.Size(); ++memberId) {
    const World::Member& member = members[memberId];
    const Ds::Vector<World::MemberId> childrenIds = member.Children();
    if (childrenIds.Size() == 0) {
      continue;
    }
    noRelationships = false;
    std::cout << '\n' << std::setfill('0') << std::setw(2) << memberId << ":";
    for (World::MemberId childId : childrenIds) {
      const World::Member& child = members[childId];
      std::cout << " [" << childId << "|" << child.Parent() << "]";
    }
  }
  if (noRelationships) {
    std::cout << "\nNone";
  }
  std::cout << '\n';
}

void PrintSpaceMembers(const World::Space& space)
{
  std::cout << "-MemberBin-";
  const Ds::Vector<World::Member>& members = space.Members();
  const size_t rowSize = 5;
  for (World::MemberId memberId = 0; memberId < members.Size(); ++memberId) {
    if (memberId % rowSize == 0) {
      std::cout << "\n"
                << std::setfill('0') << std::setw(2) << memberId << ": ";
    }
    else {
      std::cout << " ";
    }
    const World::Member& member = members[memberId];
    if (!member.InUse()) {
      std::cout << "[ inv ]";
    }
    else if (member.DescriptorCount() == 0) {
      std::cout << "[empty]";
    }
    else {
      std::cout << "[" << std::setfill('0') << std::setw(2)
                << member.FirstDescriptorId() << "|" << std::setfill('0')
                << std::setw(2) << member.LastDescriptorId() << "]";
    }
  }
  std::cout << "\n";
}

void PrintSpaceDescriptorBin(const World::Space& space)
{
  const Ds::Vector<World::ComponentDescriptor>& descriptorBin =
    space.DescriptorBin();
  const size_t rowSize = 5;
  std::cout << "-DescriptorBin-";
  for (size_t i = 0; i < descriptorBin.Size(); ++i) {
    if (i % rowSize == 0) {
      std::cout << "\n" << std::setfill('0') << std::setw(2) << i << ": ";
    }
    else {
      std::cout << " ";
    }
    const World::ComponentDescriptor& desc = descriptorBin[i];
    if (!desc.InUse()) {
      std::cout << "[inv]";
    }
    else {
      std::cout << "[" << desc.mTypeId << "|" << desc.mTableIndex << "]";
    }
  }
  std::cout << "\n";
}

void PrintSpaceUnusedMemberIds(const World::Space& space)
{
  const Ds::Vector<World::MemberId>& unusedMemberIds = space.UnusedMemberIds();
  std::cout << "UnusedMemberIds: ";
  if (unusedMemberIds.Size() == 0) {
    std::cout << "None" << std::endl;
    return;
  }
  std::cout << unusedMemberIds[0];
  for (size_t i = 1; i < unusedMemberIds.Size(); ++i) {
    std::cout << ", " << unusedMemberIds[i];
  }
  std::cout << std::endl;
}

void PrintSpace(const World::Space& space)
{
  PrintSpaceTablesStats(space);
  PrintSpaceTablesOwners(space);
  PrintSpaceMembers(space);
  PrintSpaceDescriptorBin(space);
}

#endif
