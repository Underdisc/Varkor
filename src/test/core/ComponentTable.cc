#include <iostream>

#include "core/ComponentTable.h"
#include "debug/MemLeak.h"

struct TestComp
{
  int mI;
  float mF;
  double mD;
};

void Create()
{
  std::cout << "Create" << std::endl;

  ComponentTable table;
  table.Init(sizeof(TestComp));
  const int numComponents = 15;
  std::cout << "New Component Indicies: ";
  for (int i = 0; i < numComponents; ++i)
  {
    int newCompIndex = table.Create(i);
    std::cout << newCompIndex;
    if (i < numComponents - 1)
    {
      std::cout << ", ";
    }
  }
  std::cout << std::endl;
  table.ShowStats();
  std::cout << std::endl;
}

void IndexOperator()
{
  std::cout << "IndexOperator" << std::endl;

  ComponentTable table;
  table.Init(sizeof(TestComp));

  // We create some components, but before creating all components, we set some
  // data to ensure that data is copied when the component table grows.
  const int numComponents = 15;
  const int compRangeStart = ComponentTable::smStartCapacity - 3;
  const int compRangeEnd = ComponentTable::smStartCapacity - 1;
  for (int i = 0; i < compRangeEnd; ++i)
  {
    table.Create(i);
  }
  for (int i = compRangeStart; i <= compRangeEnd; ++i)
  {
    TestComp* testCompP = (TestComp*)table[i];
    testCompP->mI = i;
    testCompP->mF = (float)i;
    testCompP->mD = (double)i;
  }
  for (int i = compRangeEnd + 1; i < numComponents; ++i)
  {
    table.Create(i);
  }

  for (int i = compRangeStart; i <= compRangeEnd; ++i)
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
  Create();
  IndexOperator();
}
