#include <iostream>
#include <random>

#include "Error.h"
#include "debug/MemLeak.h"
#include "ds/HashSet.h"
#include "test/ds/Print.h"
#include "test/ds/Test.h"

void Insert0() {
  Ds::HashSet<unsigned int> test;
  for (unsigned int i = 0; i < 10; i += 2) {
    test.Insert(i);
  }
  PrintHashSetDs(test);
  std::cout << '\n';
}

void Insert1() {
  Ds::HashSet<unsigned int> test;
  for (unsigned int i = 0; i < 80; i += 2) {
    test.Insert(i);
  }
  PrintHashSetDs(test);
  std::cout << '\n';
}

void Insert2() {
  std::mt19937 generator;
  std::uniform_int_distribution<unsigned int> distribution(0);
  Ds::HashSet<unsigned int> test;
  for (unsigned int i = 0; i < 160; ++i) {
    unsigned int newValue = distribution(generator) % 300;
    if (!test.Contains(newValue)) {
      test.Insert(newValue);
    }
  }
  PrintHashSetDs(test);
  std::cout << '\n';
}

template<>
size_t Ds::Hash(const TestType& value) {
  return (size_t)value.mA;
}

void Insert3() {
  Ds::HashSet<TestType> test;
  for (int i = 0; i < 20; ++i) {
    test.Insert(TestType(i));
  }
  PrintHashSetDs(test);
  std::cout << '\n';
  test.Clear();
  TestType::PrintCounts();
}

void Insert4() {
  std::mt19937 generator;
  std::uniform_int_distribution<unsigned int> distribution(0);
  Ds::HashSet<unsigned int> test;
  auto insertRandomValue = [&]() {
    unsigned int newValue = distribution(generator) % 500;
    if (!test.Contains(newValue)) {
      test.Insert(newValue);
    }
  };
  insertRandomValue();
  while (test.LoadFactor(test.Size() + 1) < test.smGrowLoadFactor) {
    insertRandomValue();
  }
  std::cout << "<Before Grow (BucketCount: " << test.Buckets().Size() << ")\n"
            << test << '\n';
  insertRandomValue();
  std::cout << "<After Grow (BucketCount: " << test.Buckets().Size() << ")\n"
            << test << '\n';
}

template<typename T>
Ds::HashSet<T> CreateSet(const Ds::Vector<T>& values) {
  Ds::HashSet<T> hashSet;
  for (const T& value: values) {
    hashSet.Insert(value);
  }
  return hashSet;
}

void Remove0() {
  Ds::HashSet<unsigned int> test =
    CreateSet<unsigned int>({1, 11, 3, 33, 4, 44, 8, 9, 99});
  test.Remove(11);
  test.Remove(8);
  test.Remove(9);
  PrintHashSetDs(test);
  std::cout << '\n';
}

void Remove1() {
  Ds::HashSet<TestType> test =
    CreateSet<TestType>({1, 11, 3, 33, 4, 44, 8, 9, 99});
  test.Remove(TestType(3));
  test.Remove(TestType(44));
  test.Remove(TestType(9));
  test.Remove(TestType(99));
  PrintHashSetDs(test);
  std::cout << '\n';
  TestType::PrintCounts();
}

void Contains() {
  // clang-format off
  Ds::HashSet<std::string> test = CreateSet<std::string>({"Here", "some",
    "random", "Velleicht", "words", "Yep", "auf", "Deutsch", "auch",
    "gleichzeitig", "test"});
  std::string containmentTests[] = {"Nope", "Yep", "test", "false", "true",
    "Velleicht", "Deutsch", "nicht", "auch"};
  // clang-format on
  PrintHashSetDs(test);
  std::cout << '\n';
  for (const std::string& value: containmentTests) {
    std::cout << value << ": " << test.Contains(value) << '\n';
  }
}

void Iterators() {
  Ds::HashSet<int> test = CreateSet<int>({1, 11, 31, 5, 25, 6, 8, 68, 78});
  std::cout << test << '\n';
  auto it = ++test.begin();
  std::cout << *it++ << ", ";
  std::cout << *++it << ", ";
  ++ ++ ++it;
  std::cout << *it-- << ", ";
  std::cout << *--it << '\n';

  auto cIt = ++test.cbegin();
  std::cout << *cIt++ << ", ";
  std::cout << *++cIt << ", ";
  ++ ++ ++cIt;
  std::cout << *cIt-- << ", ";
  std::cout << *--cIt << '\n';
}

int main(void) {
  Error::Init();
  EnableLeakOutput();
  RunDsTest(Insert0);
  RunDsTest(Insert1);
  RunDsTest(Insert2);
  RunDsTest(Insert3);
  RunDsTest(Insert4);
  RunDsTest(Remove0);
  RunDsTest(Remove1);
  RunDsTest(Contains);
  RunDsTest(Iterators);
}
