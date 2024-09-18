#include <iostream>

#include "comp/Type.h"
#include "debug/MemLeak.h"
#include "test/Test.h"
#include "test/world/Print.h"
#include "test/world/TestTypes.h"
#include "world/Table.h"

void Add()
{
  World::Table counter(Comp::Type<CallCounter>::smId);
  World::Table simple(Comp::Type<Simple0>::smId);
  World::Table dynamic(Comp::Type<Dynamic>::smId);
  World::Table container(Comp::Type<Container>::smId);
  for (World::MemberId i = 0; i < 15; ++i) {
    counter.Add(i);
    simple.Add(i);
    dynamic.Add(i);
    container.Add(i);
  }
  std::cout << "-Simple-\n";
  PrintTableStats(simple);
  PrintTableOwners(simple);
  std::cout << "-Dynamic-\n";
  PrintTableStats(dynamic);
  PrintTableOwners(dynamic);
  std::cout << "-Container-\n";
  PrintTableStats(container);
  PrintTableOwners(container);
}

void Remove0()
{
  World::Table table(Comp::Type<Dynamic>::smId);
  SparseId ids[2];
  ids[0] = table.Add(0);
  ids[1] = table.Add(1);
  table.Remove(0);
  PrintTable<Dynamic>(table);
}

void Remove1()
{
  World::Table table(Comp::Type<Dynamic>::smId);
  SparseId ids[3];
  ids[0] = table.Add(0);
  ids[1] = table.Add(1);
  ids[2] = table.Add(2);
  table.Remove(ids[1]);
  table.Remove(ids[0]);
  ids[0] = table.Add(0);
  PrintTable<Dynamic>(table);
}

void Remove2()
{
  World::Table tables[4] = {
    Comp::Type<CallCounter>::smId,
    Comp::Type<Simple0>::smId,
    Comp::Type<Dynamic>::smId,
    Comp::Type<Container>::smId};
  for (size_t currentTable = 0; currentTable < 4; ++currentTable) {
    World::Table& table = tables[currentTable];
    SparseId ids[10];
    for (World::MemberId i = 0; i < 10; ++i) {
      ids[i] = table.Add(i);
    }
    for (World::MemberId i = 0; i < 10; i += 2) {
      table.Remove(ids[i]);
    }
    for (World::MemberId i = 10; i < 15; ++i) {
      table.Add(i);
    }
  }
  std::cout << "-Simple-\n";
  PrintTable<Simple0>(tables[1]);
  std::cout << "-Dynamic-\n";
  PrintTable<Dynamic>(tables[2]);
  std::cout << "-Container-\n";
  PrintTable<Container>(tables[3]);
}

void Duplicate0()
{
  World::Table table(Comp::Type<Dynamic>::smId);
  SparseId componentId = table.Add(0);
  ((Dynamic*)table.GetComponent(componentId))->SetData(0);
  table.Duplicate(componentId, 1);
  PrintTable<Dynamic>(table);
}

void Duplicate1()
{
  World::Table counterTable(Comp::Type<CallCounter>::smId);
  World::Table dynamicTable(Comp::Type<Dynamic>::smId);
  World::Table containerTable(Comp::Type<Container>::smId);
  SparseId ogCounterIds[10], ogDynamicIds[10], ogContainerIds[10];
  for (World::MemberId i = 0; i < 10; ++i) {
    ogCounterIds[i] = counterTable.Add(i);
    ogDynamicIds[i] = dynamicTable.Add(i);
    ogContainerIds[i] = containerTable.Add(i);
    ((Dynamic*)dynamicTable.GetComponent(ogDynamicIds[i]))->SetData(i);
    ((Container*)containerTable.GetComponent(ogContainerIds[i]))->SetData(i);
  }
  SparseId cpDynamicIds[10], cpContainerIds[10];
  for (World::MemberId i = 0; i < 10; ++i) {
    counterTable.Duplicate(ogCounterIds[i], i + 10);
    cpDynamicIds[i] = dynamicTable.Duplicate(ogDynamicIds[i], i + 10);
    cpContainerIds[i] = containerTable.Duplicate(ogContainerIds[i], i + 10);
  }
  for (size_t i = 0; i < 10; ++i) {
    std::cout << *(Dynamic*)dynamicTable.GetComponent(ogDynamicIds[i])
              << *(Dynamic*)dynamicTable.GetComponent(cpDynamicIds[i]) << '\n'
              << *(Container*)containerTable.GetComponent(ogContainerIds[i])
              << *(Container*)containerTable.GetComponent(cpContainerIds[i])
              << '\n';
  }
}

void GetComponent()
{
  // We create some components, but before creating all components, we set
  // some data to ensure that data is copied when the tables grow.
  World::Table dynamic(Comp::Type<Dynamic>::smId);
  World::Table container(Comp::Type<Container>::smId);
  SparseId dynamicIds[World::Table::smStartCapacity],
    containerIds[World::Table::smStartCapacity];
  for (int i = 0; i < World::Table::smStartCapacity; ++i) {
    dynamicIds[i] = dynamic.Add((MemberId)i);
    containerIds[i] = container.Add((MemberId)i);
  }
  for (size_t i = 0; i < 4; ++i) {
    Dynamic* dynamicComponent = (Dynamic*)dynamic.GetComponent(dynamicIds[i]);
    dynamicComponent->SetData((int)i);
    Container* containerComponent =
      (Container*)container.GetComponent(containerIds[i]);
    containerComponent->SetData((int)i);
  }
  for (int i = World::Table::smStartCapacity; i < 15; ++i) {
    dynamic.Add((MemberId)i);
    container.Add((MemberId)i);
  }
  std::cout << "-Dynamic-\n";
  PrintTable<Dynamic>(dynamic);
  std::cout << "-Container-\n";
  PrintTable<Container>(container);
}

int main(void)
{
  Error::Init();
  RegisterComponentTypes();

  RunCallCounterTest(Add);
  RunTest(Remove0);
  RunTest(Remove1);
  RunCallCounterTest(Remove2);
  RunTest(Duplicate0);
  RunCallCounterTest(Duplicate1);
  RunTest(GetComponent);
}
