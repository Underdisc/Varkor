#include <iostream>

#include "comp/Type.h"
#include "debug/MemLeak.h"
#include "test/world/Print.h"
#include "test/world/TestTypes.h"
#include "world/Table.h"

void Add()
{
  std::cout << "<= Add =>" << std::endl;
  CallCounter::Reset();
  {
    World::Table counter(Comp::Type<CallCounter>::smId);
    World::Table simple(Comp::Type<Simple0>::smId);
    World::Table dynamic(Comp::Type<Dynamic>::smId);
    World::Table container(Comp::Type<Container>::smId);
    for (int i = 0; i < 15; ++i)
    {
      counter.Add(i);
      simple.Add(i);
      dynamic.Add(i);
      container.Add(i);
    }
    std::cout << "-Simple-" << std::endl;
    PrintTableStats(simple);
    PrintTableOwners(simple);
    std::cout << "-Dynamic-" << std::endl;
    PrintTableStats(dynamic);
    PrintTableOwners(dynamic);
    std::cout << "-Container-" << std::endl;
    PrintTableStats(container);
    PrintTableOwners(container);
  }
  CallCounter::Print();
  std::cout << std::endl;
}

void Rem()
{
  std::cout << "<= Rem =>" << std::endl;
  CallCounter::Reset();
  {
    World::Table tables[4] = {
      Comp::Type<CallCounter>::smId,
      Comp::Type<Simple0>::smId,
      Comp::Type<Dynamic>::smId,
      Comp::Type<Container>::smId};
    for (int currentTable = 0; currentTable < 4; ++currentTable)
    {
      World::Table& table = tables[currentTable];
      for (int i = 0; i < 10; ++i)
      {
        table.Add(i);
      }
      for (int i = 0; i < 10; i += 2)
      {
        table.Rem(i);
      }
      for (int i = 10; i < 15; ++i)
      {
        table.Add(i);
      }
    }
    std::cout << "-Simple-" << std::endl;
    PrintTable<Simple0>(tables[1]);
    std::cout << "-Dynamic-" << std::endl;
    PrintTable<Dynamic>(tables[2]);
    std::cout << "-Container-" << std::endl;
    PrintTable<Container>(tables[3]);
  }
  CallCounter::Print();
  std::cout << std::endl;
}

void Duplicate()
{
  std::cout << "<= Duplicate =>" << std::endl;
  CallCounter::Reset();
  {
    World::Table counterTable(Comp::Type<CallCounter>::smId);
    World::Table dynamicTable(Comp::Type<Dynamic>::smId);
    World::Table containerTable(Comp::Type<Container>::smId);
    for (int i = 0; i < 10; ++i)
    {
      counterTable.Add(i);
      int dynamicIndex = dynamicTable.Add(i);
      Dynamic* dynamicComponent = (Dynamic*)dynamicTable[dynamicIndex];
      dynamicComponent->SetData(i);
      int containerIndex = containerTable.Add(i);
      Container* containerComponent =
        (Container*)containerTable[containerIndex];
      containerComponent->SetData(i);
    }
    for (int i = 0; i < 10; ++i)
    {
      counterTable.Duplicate(i, i + 10);
      dynamicTable.Duplicate(i, i + 10);
      containerTable.Duplicate(i, i + 10);
    }
    for (int i = 0; i < 10; ++i)
    {
      Dynamic* ogDynamic = (Dynamic*)dynamicTable[i];
      Dynamic* cpDynamic = (Dynamic*)dynamicTable[i + 10];
      ogDynamic->PrintData();
      cpDynamic->PrintData();
      std::cout << std::endl;
      Container* ogContainer = (Container*)containerTable[i];
      Container* cpContainer = (Container*)containerTable[i + 10];
      ogContainer->PrintData();
      cpContainer->PrintData();
      std::cout << std::endl;
    }
  }
  CallCounter::Print();
  std::cout << std::endl;
}

void GetComponent()
{
  std::cout << "<= GetComponent =>" << std::endl;
  // We create some components, but before creating all components, we set some
  // data to ensure that data is copied when the tables grow.
  World::Table dynamic(Comp::Type<Dynamic>::smId);
  World::Table container(Comp::Type<Container>::smId);
  for (int i = 0; i < World::Table::smStartCapacity; ++i)
  {
    dynamic.Add(i);
    container.Add(i);
  }
  for (int i = 0; i < 4; ++i)
  {
    Dynamic* dynamicComponent = (Dynamic*)dynamic.GetComponent(i);
    dynamicComponent->SetData(i);
    Container* containerComponent = (Container*)container.GetComponent(i);
    containerComponent->SetData(i);
  }
  for (int i = World::Table::smStartCapacity; i < 15; ++i)
  {
    dynamic.Add(i);
    container.Add(i);
  }
  std::cout << "-Dynamic-" << std::endl;
  PrintTable<Dynamic>(dynamic);
  std::cout << "-Container-" << std::endl;
  PrintTable<Container>(container);
  std::cout << std::endl;
}

int main(void)
{
  InitMemLeakOutput();
  RegisterComponentTypes();

  Add();
  Rem();
  Duplicate();
  GetComponent();
}
