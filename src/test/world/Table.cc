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
    counter.Request(i);
    simple.Request(i);
    dynamic.Request(i);
    container.Request(i);
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

void Move()
{
  World::Table container(Comp::Type<Container>::smId);
  for (World::MemberId i = 0; i < 15; ++i) {
    ((Container*)container.Request(i))->SetData(i);
  }
  World::Table moved(std::move(container));
  PrintTable<Container>(container);
  PrintTable<Container>(moved);
}

void Remove0()
{
  World::Table table(Comp::Type<Dynamic>::smId);
  table.Request(0);
  table.Request(1);
  table.Remove(0);
  PrintTable<Dynamic>(table);
}

void Remove1()
{
  World::Table table(Comp::Type<Dynamic>::smId);
  table.Request(0);
  table.Request(1);
  table.Request(2);
  table.Remove(1);
  table.Remove(0);
  table.Request(0);
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
    for (World::MemberId i = 0; i < 10; ++i) {
      table.Request(i);
    }
    for (World::MemberId i = 0; i < 10; i += 2) {
      table.Remove(i);
    }
    for (World::MemberId i = 10; i < 15; ++i) {
      table.Request(i);
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
  ((Dynamic*)table.Request(0))->SetData(0);
  table.Duplicate(0, 1);
  PrintTable<Dynamic>(table);
}

void Duplicate1()
{
  World::Table counterTable(Comp::Type<CallCounter>::smId);
  World::Table dynamicTable(Comp::Type<Dynamic>::smId);
  World::Table containerTable(Comp::Type<Container>::smId);
  for (World::MemberId i = 0; i < 10; ++i) {
    counterTable.Request(i);
    ((Dynamic*)dynamicTable.Request(i))->SetData(i);
    ((Container*)containerTable.Request(i))->SetData(i);
  }
  SparseId cpDynamicIds[10], cpContainerIds[10];
  for (World::MemberId i = 0; i < 10; ++i) {
    counterTable.Duplicate(i, i + 10);
    dynamicTable.Duplicate(i, i + 10);
    containerTable.Duplicate(i, i + 10);
  }
  for (size_t i = 0; i < 10; ++i) {
    std::cout << *(Dynamic*)dynamicTable.GetComponent(i)
              << *(Dynamic*)dynamicTable.GetComponent(i + 10) << '\n'
              << *(Container*)containerTable.GetComponent(i)
              << *(Container*)containerTable.GetComponent(i + 10) << '\n';
  }
}

void GetComponent()
{
  // We create some components, but before creating all components, we set
  // some data to ensure that data is copied when the tables grow.
  World::Table dynamic(Comp::Type<Dynamic>::smId);
  World::Table container(Comp::Type<Container>::smId);
  for (MemberId i = 0; i < World::Table::smStartCapacity; ++i) {
    dynamic.Request(i);
    container.Request(i);
  }
  for (size_t i = 0; i < 4; ++i) {
    ((Dynamic*)dynamic.GetComponent(i))->SetData((int)i);
    ((Container*)container.GetComponent(i))->SetData((int)i);
  }
  for (MemberId i = World::Table::smStartCapacity; i < 15; ++i) {
    dynamic.Request(i);
    container.Request(i);
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
  RunTest(Move);
  RunTest(Remove0);
  RunTest(Remove1);
  RunCallCounterTest(Remove2);
  RunTest(Duplicate0);
  RunCallCounterTest(Duplicate1);
  RunTest(GetComponent);
}
