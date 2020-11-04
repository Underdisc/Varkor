#include <iostream>

#include "core/ComponentTable.h"
#include "debug/MemLeak.h"

struct TestComp
{
  int mI;
  float mF;
  double mD;
};

void Add()
{
  std::cout << "Add" << std::endl;

  Core::ComponentTable table(sizeof(TestComp));
  int newCompIndex = table.Add(0);
  std::cout << "[" << newCompIndex;
  for (int i = 1; i < 15; ++i)
  {
    newCompIndex = table.Add(i);
    std::cout << ", " << newCompIndex;
  }
  std::cout << "]" << std::endl;
  table.ShowOwners();
  table.ShowStats();
  std::cout << std::endl;
}

void Rem()
{
  std::cout << "Rem" << std::endl;
  Core::ComponentTable table(sizeof(TestComp));
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

  table.ShowOwners();
  table.ShowStats();
  std::cout << std::endl;
}

void IndexOperator()
{
  std::cout << "IndexOperator" << std::endl;

  // We create some components, but before creating all components, we set some
  // data to ensure that data is copied when the component table grows.
  Core::ComponentTable table(sizeof(TestComp));
  const int numComponents = 15;
  const int compRangeStart = Core::ComponentTable::smStartCapacity - 3;
  const int compRangeEnd = Core::ComponentTable::smStartCapacity;
  for (int i = 0; i < compRangeEnd; ++i)
  {
    table.Add(i);
  }
  for (int i = compRangeStart; i < compRangeEnd; ++i)
  {
    TestComp* testCompP = (TestComp*)table[i];
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
    TestComp* testCompP = (TestComp*)table[i];
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
  IndexOperator();
}
