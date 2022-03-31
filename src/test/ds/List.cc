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
  for (int i = 0; i < 10; ++i) {
    TestType testType(i);
    list.PushBack(testType);
    list.PushFront(testType);
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
  for (int i = 0; i < 20; ++i) {
    list.PushBack(TestType(i));
    list.PushFront(TestType(i));
  }
  for (int i = 0; i < 10; ++i) {
    list.PopBack();
    list.PopFront();
  }
  for (int i = 0; i < 5; ++i) {
    list.PushBack(TestType(i));
    list.PushFront(TestType(i));
  }
  PrintList(list);
  for (int i = 0; i < 30; ++i) {
    list.PopBack();
  }
  PrintList(list);
  TestType::PrintCounts();
}

void Iter()
{
  std::cout << "<= Iter =>\n";
  Ds::List<TestType> list;
  for (int i = 0; i < 5; ++i) {
    list.EmplaceFront(i);
  }

  // Print elements using a const iterator.
  auto cIt = list.cbegin();
  auto cItE = list.cend();
  std::cout << "List: [" << *cIt;
  ++cIt;
  while (cIt != cItE) {
    std::cout << ", " << cIt->mA;
    ++cIt;
  }
  std::cout << "]\n";

  // Print elements using a non-const iterator.
  auto it = list.begin();
  auto itE = list.end();
  std::cout << "List: [" << *it;
  ++it;
  while (it != itE) {
    std::cout << ", " << it->mA;
    ++it;
  }
  std::cout << "]\n";

  for (int i = 0; i < 5; ++i) {
    list.PopFront();
  }
  std::cout << "Empty: " << (list.begin() == list.end()) << "\n";
}

void EmplaceBackFront()
{
  std::cout << "<= EmplaceBackFront =>\n";
  Ds::List<TestType> list;
  for (int i = 0; i < 10; ++i) {
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
  for (int i = 0; i < 10; ++i) {
    TestType testType(i);
    list.PushBack(Util::Move(testType));
    list.PushFront(Util::Move(testType));
  }
  PrintList(list);
  list.Clear();
  TestType::PrintCounts();
}

void Insert()
{
  std::cout << "<= Insert =>\n";
  Ds::List<TestType> list;
  for (int i = 0; i < 10; ++i) {
    list.EmplaceBack(i);
  }
  auto it = list.begin();
  auto itE = list.end();
  while (it != itE) {
    TestType testType(*it);
    list.Insert(it, testType);
    list.Insert(it, Util::Move(testType));
    ++it;
  }
  for (int i = 0; i < 5; ++i) {
    list.Insert(list.end(), TestType(i));
  }
  PrintList(list);
  list.Clear();
  TestType::PrintCounts();
}

void Emplace()
{
  std::cout << "<= Emplace =>\n";
  Ds::List<TestType> list;
  for (int i = 0; i < 10; ++i) {
    list.EmplaceBack(TestType(i));
  }
  int newValue = 9;
  auto it = list.begin();
  auto itE = list.end();
  while (it != itE) {
    list.Emplace(it, newValue);
    ++it;
    --newValue;
  }
  PrintList(list);
  list.Clear();
  TestType::PrintCounts();
}

void Erase()
{
  std::cout << "<= Erase =>\n";
  Ds::List<TestType> list;
  for (int i = 0; i < 10; ++i) {
    list.EmplaceBack(i);
  }

  // Erase every other element.
  auto it = list.begin();
  auto itE = list.end();
  while (it != itE) {
    it = list.Erase(it);
    ++it;
  }
  PrintList(list);

  // Erase the last element.
  it = list.begin();
  for (int i = 0; i < 4; ++i) {
    ++it;
  }
  it = list.Erase(it);
  LogAbortIf(it != list.end(), "Iterator must be the end of the list.");
  PrintList(list);

  // Erase the remaining elements.
  it = list.begin();
  while (it != itE) {
    it = list.Erase(it);
  }
  PrintList(list);
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
  RunTest(EmplaceBackFront);
  RunTest(MovePush);
  RunTest(Insert);
  RunTest(Emplace);
  RunTest(Erase);
}
