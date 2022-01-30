#include <iomanip>
#include <iostream>

#include "debug/MemLeak.h"
#include "ds/List.h"
#include "test/ds/Print.h"
#include "test/ds/TestType.h"
#include "util/Utility.h"

void Push()
{
  std::cout << "<= Push =>\n";
  Ds::List<TestType> list;
  for (int i = 0; i < 10; ++i)
  {
    list.PushBack(TestType(i));
    list.PushFront(TestType(i));
  }
  PrintList(list);
  list.Clear();
  PrintList(list);
  TestType::PrintCounts();
}

void Pop()
{
  std::cout << "<= Pop =>\n";
  Ds::List<TestType> list;
  for (int i = 0; i < 20; ++i)
  {
    list.PushBack(TestType(i));
    list.PushFront(TestType(i));
  }
  for (int i = 0; i < 10; ++i)
  {
    list.PopBack();
    list.PopFront();
  }
  for (int i = 0; i < 5; ++i)
  {
    list.PushBack(TestType(i));
    list.PushFront(TestType(i));
  }
  PrintList(list);
  for (int i = 0; i < 30; ++i)
  {
    list.PopBack();
  }
  PrintList(list);
  TestType::PrintCounts();
}

void Iter()
{
  std::cout << "<= Iter =>\n";
  Ds::List<int> list;
  for (int i = 0; i < 10; ++i)
  {
    list.PushFront(i);
  }
  auto it = list.begin();
  auto itE = list.end();
  std::cout << "-ListNonConstIter-\n";
  std::cout << "[" << *it;
  ++it;
  while (it != itE)
  {
    std::cout << ", " << *it;
    ++it;
  }
  std::cout << "]\n";
  for (int i = 0; i < 10; ++i)
  {
    list.PopFront();
  }
  PrintList(list);
}

void Emplace()
{
  std::cout << "<= Emplace =>\n";
  Ds::List<TestType> list;
  for (int i = 0; i < 10; ++i)
  {
    list.EmplaceBack(i);
    list.EmplaceFront(i);
  }
  PrintList(list);
  list.Clear();
  TestType::PrintCounts();
}

void MovePush()
{
  std::cout << "<= MovePush =>\n";
  Ds::List<TestType> list;
  for (int i = 0; i < 10; ++i)
  {
    TestType testType(i);
    list.PushBack(Util::Move(testType));
    list.PushFront(Util::Move(testType));
  }
  PrintList(list);
  list.Clear();
  TestType::PrintCounts();
}

void RunTest(void (*testFunction)())
{
  TestType::ResetCounts();
  testFunction();
  std::cout << "\n";
}

void main()
{
  EnableLeakOutput();
  RunTest(Push);
  RunTest(Pop);
  RunTest(Iter);
  RunTest(Emplace);
  RunTest(MovePush);
}
