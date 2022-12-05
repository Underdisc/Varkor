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
  std::cout << "<= InsertRemoveTryGet =>" << std::endl;
  // Start by inserting a chunk a random keys with incrementing values.
  Ds::Map<int, int> map;
  constexpr int maxInsertions = 50;
  int insertedValues[maxInsertions];
  int insertionCount = 0;
  srand(1);
  while (insertionCount < maxInsertions) {
    int value = rand() % 1000;
    if (!map.Contains(value)) {
      map.Insert(value, insertionCount);
      insertedValues[insertionCount] = value;
      ++insertionCount;
    }
  }
  PrintMap(map);
  std::cout << "--- 0 ---" << std::endl;

  // Remove half of the values that were just inserted.
  int removalCount = maxInsertions / 2;
  for (int i = 0; i < removalCount; ++i) {
    map.Remove(insertedValues[i]);
  }
  PrintMap(map);
  std::cout << "--- 1 ---" << std::endl;

  // Attempt to get values for keys that have been either removed or not
  // removed from the map.
  for (int i = 0; i < maxInsertions; i += 2) {
    std::cout << i << "->" << insertedValues[i] << ": ";
    int* value = map.TryGet(insertedValues[i]);
    if (value == nullptr) {
      std::cout << "not found" << std::endl;
    }
    else {
      std::cout << *value << std::endl;
    }
  }
  std::cout << std::endl;
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
  std::cout << "<= Iterator =>" << std::endl;
  // Insert a chunk of random key value pairs into the map.
  Ds::Map<int, int> map;
  int insertionCount = 0;
  srand(20);
  while (insertionCount < 20) {
    int key = rand() % 100;
    if (!map.Contains(key)) {
      map.Insert(key, insertionCount);
      ++insertionCount;
    }
  }
  PrintMap(map);

  // Change the value of all the key value pairs using an iterator.
  auto it = map.begin();
  auto itE = map.end();
  int iterationCount = 0;
  while (it != itE) {
    it->mValue = iterationCount;
    ++iterationCount;
    ++it;
  }
  std::cout << "--- 0 ---" << std::endl;
  PrintMap(map);

  // Print out all the keys using a const iterator.
  std::cout << "--- 1 ---" << std::endl;
  auto cIt = map.cbegin();
  auto cItE = map.cend();
  while (cIt != cItE) {
    std::cout << cIt->Key() << ": " << *cIt << std::endl;
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
