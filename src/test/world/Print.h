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
void PrintSpaceComponentData(std::ostream& os, const World::Space& space)
{
  std::stringstream output;
  Ds::Vector<World::MemberId> slice = space.Slice<T>();
  for (int i = 0; i < slice.Size(); ++i) {
    output << space.Get<T>(slice[i]);
  }
  std::string outputString = output.str();
  if (!outputString.empty()) {
    os << Comp::Type<T>::smId << ": " << outputString << '\n';
  }
}

void PrintSpaceTestTypeComponentData(const World::Space& space)
{
  std::stringstream output;
  PrintSpaceComponentData<Simple0>(output, space);
  PrintSpaceComponentData<Simple1>(output, space);
  PrintSpaceComponentData<Dynamic>(output, space);
  PrintSpaceComponentData<Container>(output, space);
  PrintSpaceComponentData<Dependant>(output, space);
  std::string outputString = output.str();
  if (!outputString.empty()) {
    std::cout << "-ComponentData-\n" << outputString;
  }
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

void PrintChildren(
  const World::Space& space,
  MemberId memberId,
  std::stringstream* output,
  std::string indent = "")
{
  auto* relationship = space.TryGet<Comp::Relationship>(memberId);
  if (relationship == nullptr || relationship->mChildren.Empty()) {
    return;
  }
  for (int i = 0; i < relationship->mChildren.Size(); ++i) {
    MemberId childId = relationship->mChildren[i];
    *output << indent << "\\-" << childId << '\n';
    std::string newIndent = indent;
    if (i < relationship->mChildren.Size() - 1) {
      newIndent += "| ";
    }
    else {
      newIndent += "  ";
    }
    PrintChildren(space, childId, output, newIndent);
  }
}

void PrintSpaceRelationships(const World::Space& space)
{
  std::stringstream output;
  const Ds::Vector<MemberId> rootMembers = space.RootMemberIds();
  for (MemberId rootMemberId : rootMembers) {
    auto* relationship = space.TryGet<Comp::Relationship>(rootMemberId);
    if (relationship == nullptr) {
      continue;
    }
    output << rootMemberId << '\n';
    PrintChildren(space, rootMemberId, &output);
  }
  std::string outputStr = output.str();
  if (!outputStr.empty()) {
    std::cout << "-Relationships-" << '\n' << outputStr;
  }
}

void PrintSpaceMembers(const World::Space& space)
{
  std::cout << "-MemberPool- [memberId|firstDescriptorId|descriptorCount]";
  const Ds::Pool<World::Member>& members = space.Members();
  const size_t rowSize = 5;
  for (int i = 0; i < members.Size(); ++i) {
    const World::Member& member = members.GetWithDenseIndex(i);
    if (i % rowSize == 0) {
      std::cout << '\n' << std::setfill('0') << std::setw(2) << i << ": ";
    }
    else {
      std::cout << ' ';
    }

    Ds::PoolId memberId = members.Dense()[i];
    std::cout << '[' << std::setfill('0') << std::setw(2) << memberId << '|'
              << std::setfill('0') << std::setw(2) << member.FirstDescriptorId()
              << '|' << std::setfill('0') << std::setw(2)
              << member.DescriptorCount() << ']';
  }
  std::cout << '\n';
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

void PrintSpace(const World::Space& space)
{
  PrintSpaceTablesOwners(space);
  PrintSpaceMembers(space);
  PrintSpaceDescriptorBin(space);
  PrintSpaceRelationships(space);
  PrintSpaceTestTypeComponentData(space);
}

#endif
