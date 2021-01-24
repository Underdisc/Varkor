#include <iostream>
#include <stdlib.h>
#include <string>

#include "ds/Map.h"
#include "test/ds/Print.h"
#include "test/ds/TestType.h"
#include "util/Utility.h"

void InsertRemoveFind()
{
  std::cout << "<= InsertRemoveFind =>" << std::endl;
  // Start by inserting a chunk a random keys with incrementing values.
  Ds::Map<int, int> map;
  constexpr int maxInsertions = 50;
  int insertedValues[maxInsertions];
  int insertionCount = 0;
  srand(1);
  while (insertionCount < maxInsertions)
  {
    int value = rand() % 1000;
    if (!map.Contains(value))
    {
      map.Insert(value, insertionCount);
      insertedValues[insertionCount] = value;
      ++insertionCount;
    }
  }
  PrintMap(map);
  std::cout << "--- 0 ---" << std::endl;

  // Remove half of the values that were just inserted.
  int removalCount = maxInsertions / 2;
  for (int i = 0; i < removalCount; ++i)
  {
    map.Remove(insertedValues[i]);
  }
  PrintMap(map);
  std::cout << "--- 1 ---" << std::endl;

  // Attempt to find values for keys that have been either removed or not
  // removed from the map.
  for (int i = 0; i < maxInsertions; i += 2)
  {
    std::cout << i << "->" << insertedValues[i] << ": ";
    int* value = map.Find(insertedValues[i]);
    if (value == nullptr)
    {
      std::cout << "not found" << std::endl;
    } else
    {
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
  for (int i = 0; i < sequenceSize; ++i)
  {
    map.Insert(sequence[i], i);
  }
  PrintMap(map);
  std::cout << "--- 0 ---" << std::endl;

  // After inserting, we try finding a series of strings. Some of them will be
  // in the map and others won't.
  const char* findSequence[] = {
    "here", "are", "some", "values", "to", "search", "for", "in", "the", "map"};
  int findSequenceSize = sizeof(findSequence) / sizeof(char*);
  for (int i = 0; i < findSequenceSize; ++i)
  {
    std::cout << findSequence[i] << ": ";
    int* value = map.Find(findSequence[i]);
    if (value == nullptr)
    {
      std::cout << " not found" << std::endl;
    } else
    {
      std::cout << *value << std::endl;
    }
  }
  std::cout << "--- 1 ---" << std::endl;

  // Remove all of the values in the map.
  for (int i = 0; i < sequenceSize; ++i)
  {
    map.Remove(sequence[i]);
  }
  PrintMap(map);
  std::cout << std::endl;
}

void MoveInsert()
{
  std::cout << "<= MoveInsert =>" << std::endl;
  Ds::Map<int, TestType> map;
  // We perfom ten insertions using move and ten without the use of move to make
  // sure the correct constructor types are called.
  for (int i = 0; i < 10; ++i)
  {
    TestType newTestType(i, (float)i);
    map.Insert(i, Util::Move(newTestType));
  }
  for (int i = 10; i < 20; ++i)
  {
    TestType newTestType(i, (float)i);
    map.Insert(i, newTestType);
  }
  PrintMap(map);
  map.Clear();
  std::cout << "--- 0 ---" << std::endl;
  std::cout << "ConstructorCount: " << TestType::smConstructorCount << std::endl
            << "MoveConstructorCount: " << TestType::smMoveConstructorCount
            << std::endl
            << "CopyConstructorCount: " << TestType::smCopyConstructorCount
            << std::endl
            << "DestructorCount: " << TestType::smDestructorCount << std::endl;
  TestType::ResetCounts();
}

int main()
{
  InsertRemoveFind();
  StringMap();
  MoveInsert();
}
