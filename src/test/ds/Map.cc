#include <iostream>
#include <stdlib.h>
#include <string>

#include "debug/MemLeak.h"
#include "ds/Map.h"
#include "test/ds/Print.h"
#include "test/ds/TestType.h"
#include "util/Utility.h"

void InsertRemoveTryGet()
{
  std::cout << "<= InsertRemoveTryGet =>\n";
  // Start by inserting random keys with incrementing values.
  Ds::Map<int, int> map;
  constexpr int insertions = 50;
  int keys[insertions] = {41,  467, 334, 500, 169, 724, 478, 358, 962, 464,
                          705, 145, 281, 827, 961, 491, 995, 942, 436, 391,
                          604, 902, 153, 292, 382, 421, 716, 718, 895, 447,
                          726, 771, 538, 869, 912, 667, 299, 35,  894, 703,
                          811, 322, 333, 673, 664, 141, 711, 253, 868, 547};
  for (int i = 0; i < insertions; ++i) {
    map.Insert(keys[i], i);
  }
  PrintMap(map);

  // Remove half of the values that were just inserted.
  std::cout << "--- 0 ---\n";
  int removalCount = insertions / 2;
  for (int i = 0; i < removalCount; ++i) {
    map.Remove(keys[i]);
  }
  PrintMap(map);

  // Attempt to get values for keys that have been either removed or not
  // removed from the map.
  std::cout << "--- 1 ---\n";
  for (int i = 0; i < insertions; i += 2) {
    std::cout << i << "->" << keys[i] << ": ";
    int* value = map.TryGet(keys[i]);
    if (value == nullptr) {
      std::cout << "not found\n";
    }
    else {
      std::cout << *value << '\n';
    }
  }
  std::cout << '\n';
}

void StringMap()
{
  std::cout << "<= StringMap =>" << std::endl;
  // Add a sequence of strings to a map.
  Ds::Map<std::string, int> map;
  const char* sequence[] = {
    "here", "are", "some", "values", "to", "insert", "into", "the", "map"};
  int sequenceSize = sizeof(sequence) / sizeof(char*);
  for (int i = 0; i < sequenceSize; ++i) {
    map.Insert(sequence[i], i);
  }
  PrintMap(map);
  std::cout << "--- 0 ---" << std::endl;

  // Try to get values paired with key strings. Some key strings will be in the
  // map and others won't.
  const char* findSequence[] = {
    "here", "are", "some", "values", "to", "search", "for", "in", "the", "map"};
  int findSequenceSize = sizeof(findSequence) / sizeof(char*);
  for (int i = 0; i < findSequenceSize; ++i) {
    std::cout << findSequence[i] << ": ";
    int* value = map.TryGet(findSequence[i]);
    if (value == nullptr) {
      std::cout << " not found" << std::endl;
    }
    else {
      std::cout << *value << std::endl;
    }
  }
  std::cout << "--- 1 ---" << std::endl;

  // Remove all of the values in the map.
  for (int i = 0; i < sequenceSize; ++i) {
    map.Remove(sequence[i]);
  }
  PrintMap(map);
  std::cout << std::endl;
}

void InsertEmplace()
{
  std::cout << "<= InsertEmplace =>" << std::endl;
  // This will test the usage of move, copy, and special constructors when
  // inserting or emplacing. It will also test writing to newly added elements.
  Ds::Map<int, TestType> map;
  for (int i = 0; i < 10; ++i) {
    TestType& newTestType = map.Insert(i, TestType(0));
    newTestType.Set(i);
  }
  for (int i = 10; i < 15; ++i) {
    TestType toInsert(0);
    TestType& newTestType = map.Insert(i, toInsert);
    newTestType.Set(i);
  }
  for (int i = 15; i < 20; ++i) {
    TestType toEmplace(0);
    TestType& newTestType = map.Emplace(i, toEmplace);
    newTestType.Set(i);
  }
  for (int i = 20; i < 30; ++i) {
    map.Emplace(i, i, (float)i);
  }
  PrintMap(map);
  map.Clear();
  std::cout << "--- 0 ---" << std::endl;
  std::cout << "ConstructorCount: " << TestType::smConstructorCount << std::endl
            << "MoveConstructorCount: " << TestType::smMoveConstructorCount
            << std::endl
            << "CopyConstructorCount: " << TestType::smCopyConstructorCount
            << std::endl
            << "DestructorCount: " << TestType::smDestructorCount << std::endl
            << std::endl;
  TestType::ResetCounts();
}

void Iterator()
{
  std::cout << "<= Iterator =>\n";
  // Insert random key value pairs into the map.
  Ds::Map<int, int> map;
  constexpr int insertions = 20;
  int keys[insertions] = {3,  79, 73, 51, 38, 95, 78, 8,  2,  55,
                          81, 36, 27, 28, 35, 16, 43, 21, 10, 30};
  for (int i = 0; i < insertions; ++i) {
    map.Insert(keys[i], i);
  }
  PrintMap(map);

  // Change the value of all the key value pairs using an iterator.
  std::cout << "--- 0 ---\n";
  auto it = map.begin();
  auto itE = map.end();
  int iterationCount = 0;
  while (it != itE) {
    it->mValue = iterationCount;
    ++iterationCount;
    ++it;
  }
  PrintMap(map);

  // Print out all the keys using a const iterator.
  std::cout << "--- 1 ---\n";
  auto cIt = map.cbegin();
  auto cItE = map.cend();
  while (cIt != cItE) {
    std::cout << cIt->Key() << ": " << *cIt << '\n';
    ++cIt;
  }
}

int main()
{
  EnableLeakOutput();
  InsertRemoveTryGet();
  StringMap();
  InsertEmplace();
  Iterator();
}
