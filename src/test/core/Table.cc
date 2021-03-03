#include <iostream>

#include "core/Table.h"
#include "debug/MemLeak.h"
#include "test/core/Print.h"

struct TestComp
{
  int mI;
  float mF;
  double mD;
};

void Add()
{
  std::cout << "<= Add =>" << std::endl;
  World::Table table(sizeof(TestComp));
  int newCompIndex = table.Add(0);
  std::cout << "[" << newCompIndex;
  for (int i = 1; i < 15; ++i)
  {
    newCompIndex = table.Add(i);
    std::cout << ", " << newCompIndex;
  }
  std::cout << "]" << std::endl;

  PrintTableOwners(table);
  PrintTableStats(table);
  std::cout << std::endl;
}

void Rem()
{
  std::cout << "<= Rem =>" << std::endl;
  World::Table table(sizeof(TestComp));
  for (int i = 0; i < 10; ++i)
  {
    table.Add(i);
  }
  for (int i = 0; i < 10; i += 2)
  {
    table.Rem(i);
  }
  for (int i = 10; i < 25; ++i)
  {
    table.Add(i);
  }

  PrintTableOwners(table);
  PrintTableStats(table);
  std::cout << std::endl;
}

void GetData()
{
  std::cout << "<= GetData =>" << std::endl;
  // We create some components, but before creating all components, we set some
  // data to ensure that data is copied when the table grows.
  World::Table table(sizeof(TestComp));
  const int numComponents = 15;
  const int compRangeStart = World::Table::smStartCapacity - 3;
  const int compRangeEnd = World::Table::smStartCapacity;
  for (int i = 0; i < compRangeEnd; ++i)
  {
    table.Add(i);
  }
  for (int i = compRangeStart; i < compRangeEnd; ++i)
  {
    TestComp* testCompP = (TestComp*)table.GetData(i);
    testCompP->mI = i;
    testCompP->mF = (float)i;
    testCompP->mD = (double)i;
  }
  for (int i = compRangeEnd + 1; i < numComponents; ++i)
  {
    table.Add(i);
  }

  for (int i = compRangeStart; i < compRangeEnd; ++i)
  {
    TestComp* testCompP = (TestComp*)table.GetData(i);
    std::cout << "TestComp" << i << std::endl
              << "mI: " << testCompP->mI << std::endl
              << "mF: " << testCompP->mF << std::endl
              << "mD: " << testCompP->mD << std::endl;
  }
}

int main(void)
{
  InitMemLeakOutput();
  Add();
  Rem();
  GetData();
}
