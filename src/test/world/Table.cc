#include <iostream>

#include "comp/Type.h"
#include "debug/MemLeak.h"
#include "test/Test.h"
#include "test/world/Print.h"
#include "test/world/TestTypes.h"
#include "world/Table.h"

void Add()
{
  CallCounter::Reset();
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
  CallCounter::Print();
}

void Rem()
{
  CallCounter::Reset();
  {
    World::Table tables[4] = {
      Comp::Type<CallCounter>::smId,
      Comp::Type<Simple0>::smId,
      Comp::Type<Dynamic>::smId,
      Comp::Type<Container>::smId};
    for (size_t currentTable = 0; currentTable < 4; ++currentTable) {
      World::Table& table = tables[currentTable];
      for (World::MemberId i = 0; i < 10; ++i) {
        table.Add(i);
      }
      for (World::MemberId i = 0; i < 10; i += 2) {
        table.Rem(i);
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
  CallCounter::Print();
}

void Duplicate()
{
  CallCounter::Reset();
  {
    World::Table counterTable(Comp::Type<CallCounter>::smId);
    World::Table dynamicTable(Comp::Type<Dynamic>::smId);
    World::Table containerTable(Comp::Type<Container>::smId);
    for (World::MemberId i = 0; i < 10; ++i) {
      counterTable.Add(i);
      size_t dynamicIndex = dynamicTable.Add(i);
      Dynamic* dynamicComponent = (Dynamic*)dynamicTable[dynamicIndex];
      dynamicComponent->SetData(i);
      size_t containerIndex = containerTable.Add(i);
      Container* containerComponent =
        (Container*)containerTable[containerIndex];
      containerComponent->SetData(i);
    }
    for (World::MemberId i = 0; i < 10; ++i) {
      counterTable.Duplicate(i, i + 10);
      dynamicTable.Duplicate(i, i + 10);
      containerTable.Duplicate(i, i + 10);
    }
    for (size_t i = 0; i < 10; ++i) {
      Dynamic* ogDynamic = (Dynamic*)dynamicTable[i];
      Dynamic* cpDynamic = (Dynamic*)dynamicTable[i + 10];
      ogDynamic->PrintData();
      cpDynamic->PrintData();
      std::cout << '\n';
      Container* ogContainer = (Container*)containerTable[i];
      Container* cpContainer = (Container*)containerTable[i + 10];
      ogContainer->PrintData();
      cpContainer->PrintData();
      std::cout << '\n';
    }
  }
  CallCounter::Print();
}

void GetComponent()
{
  // We create some components, but before creating all components, we set some
  // data to ensure that data is copied when the tables grow.
  World::Table dynamic(Comp::Type<Dynamic>::smId);
  World::Table container(Comp::Type<Container>::smId);
  for (World::MemberId i = 0; i < World::Table::smStartCapacity; ++i) {
    dynamic.Add(i);
    container.Add(i);
  }
  for (size_t i = 0; i < 4; ++i) {
    Dynamic* dynamicComponent = (Dynamic*)dynamic.GetComponent(i);
    dynamicComponent->SetData((int)i);
    Container* containerComponent = (Container*)container.GetComponent(i);
    containerComponent->SetData((int)i);
  }
  for (World::MemberId i = World::Table::smStartCapacity; i < 15; ++i) {
    dynamic.Add(i);
    container.Add(i);
  }
  std::cout << "-Dynamic-\n";
  PrintTable<Dynamic>(dynamic);
  std::cout << "-Container-\n";
  PrintTable<Container>(container);
}

int main(void)
{
  EnableLeakOutput();
  RegisterComponentTypes();

  RunTest(Add);
  RunTest(Rem);
  RunTest(Duplicate);
  RunTest(GetComponent);
}
