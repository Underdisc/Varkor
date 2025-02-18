#include <iomanip>
#include <iostream>
#include <utility>

#include "debug/MemLeak.h"
#include "ds/List.h"
#include "test/ds/Print.h"
#include "test/ds/Test.h"
#include "test/ds/TestType.h"

void Push() {
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

void Pop() {
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

void Iter() {
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

void EmplaceBackFront() {
  Ds::List<TestType> list;
  for (int i = 0; i < 10; ++i) {
    list.EmplaceBack(i);
    list.EmplaceFront(i);
  }
  PrintList(list);
  list.Clear();
  TestType::PrintCounts();
}

void MovePush() {
  Ds::List<TestType> list;
  for (int i = 0; i < 10; ++i) {
    TestType testType(i);
    list.PushBack(std::move(testType));
    list.PushFront(std::move(testType));
  }
  PrintList(list);
  list.Clear();
  TestType::PrintCounts();
}

void Insert() {
  Ds::List<TestType> list;
  for (int i = 0; i < 10; ++i) {
    list.EmplaceBack(i);
  }
  auto it = list.begin();
  auto itE = list.end();
  while (it != itE) {
    TestType testType(*it);
    list.Insert(it, testType);
    list.Insert(it, std::move(testType));
    ++it;
  }
  for (int i = 0; i < 5; ++i) {
    list.Insert(list.end(), TestType(i));
  }
  PrintList(list);
  list.Clear();
  TestType::PrintCounts();
}

void Emplace() {
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

void Erase() {
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

int main() {
  EnableLeakOutput();
  RunDsTest(Push);
  RunDsTest(Pop);
  RunDsTest(Iter);
  RunDsTest(EmplaceBackFront);
  RunDsTest(MovePush);
  RunDsTest(Insert);
  RunDsTest(Emplace);
  RunDsTest(Erase);
}
