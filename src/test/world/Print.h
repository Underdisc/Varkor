#ifndef test_world_Print_h
#define test_world_Print_h

#include <iomanip>
#include <iostream>

#include "comp/Type.h"
#include "test/world/TestTypes.h"
#include "world/Space.h"
#include "world/Table.h"

template<typename T>
void PrintRegistration() {
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

template<typename T>
void PrintTable(const World::Table& table) {
  std::cout << "-TableData- [owner, data]\n";
  for (size_t i = 0; i < table.Size(); ++i) {
    std::cout << "[" << table.GetOwnerAtDenseIndex(i) << ", "
              << *(T*)table.GetComponentAtDenseIndex(i) << "]\n";
  }
}

void PrintTableStats(const World::Table& table) {
  std::cout << "-TableStats-\n"
            << "Stride: " << table.Stride() << ", Size: " << table.Size()
            << ", Capacity: " << table.Capacity() << "\n";
}

void PrintTableOwners(const World::Table& table) {
  std::cout << "-TableOwners-\n[";
  for (size_t i = 0; i < table.Size(); ++i) {
    std::cout << table.GetOwnerAtDenseIndex(i);
    if (i < table.Size() - 1) {
      std::cout << ", ";
    }
  }
  std::cout << "]\n";
}

void PrintSpaceTablesStats(const World::Space& space) {
  std::cout << "-TableStats-\n";
  const Ds::Pool<World::Table>& tables = space.Tables();
  for (Ds::PoolId id = 0; id < tables.Capacity(); ++id) {
    if (!tables.Valid(id)) {
      continue;
    }
    const World::Table& table = tables[id];
    std::cout << id << ": Stride: " << table.Stride()
              << ", Size: " << table.Size()
              << ", Capacity: " << table.Capacity() << "\n";
  }
}

void PrintSpaceTablesOwners(const World::Space& space) {
  std::cout << "-TableOwners-\n";
  const Ds::Pool<World::Table>& tables = space.Tables();
  for (Ds::PoolId id = 0; id < tables.Capacity(); ++id) {
    if (!tables.Valid(id)) {
      continue;
    }
    const World::Table& table = tables[id];
    std::cout << id << ": [";
    for (size_t i = 0; i < table.Size(); ++i) {
      std::cout << table.GetOwnerAtDenseIndex(i);
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
  std::string indent = "") {
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

void PrintSpaceRelationships(const World::Space& space) {
  std::stringstream output;
  const Ds::Vector<MemberId> rootMembers = space.RootMemberIds();
  for (MemberId rootMemberId: rootMembers) {
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

void PrintSpace(const World::Space& space) {
  std::cout << "-Space-\n";
  Vlk::Value spaceVal;
  space.Serialize(spaceVal);
  std::cout << spaceVal << '\n';
}

#endif
