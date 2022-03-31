#include <iostream>
#include <stdlib.h>
#include <string>

#include "Error.h"
#include "debug/MemLeak.h"
#include "ds/RbTree.h"
#include "test/ds/Print.h"
#include "test/ds/TestType.h"
#include "util/Utility.h"

void LeftInsert()
{
  // Every time a value is inserted, it will become the leftmost leaf node.
  std::cout << "<= LeftInsert =>" << std::endl;
  Ds::RbTree<int> tree;
  for (int i = 20; i > 0; --i) {
    tree.Insert(i);
  }
  PrintRbTree(tree);
  std::cout << std::endl;
}

void RightInsert()
{
  // Every time a value is inserted, it will become the rightmost leaf node.
  std::cout << "<= RightInsert =>" << std::endl;
  Ds::RbTree<int> tree;
  for (int i = 0; i < 20; ++i) {
    tree.Insert(i);
  }
  PrintRbTree(tree);
  std::cout << std::endl;
}

void ExplicitInsert()
{
  // The insertions for this tree make use of every possible type of
  // transforation used when balancing the tree.
  std::cout << "<= ExplicitInsert =>" << std::endl;
  int sequence[] = {18, 5,  10, 15, 16, 9,  6,  4,  2, 19, 20, 21,
                    22, 23, 24, 25, 26, 27, 28, 29, 1, 3,  0};
  int sequenceSize = sizeof(sequence) / sizeof(int);
  Ds::RbTree<int> tree;
  for (int i = 0; i < sequenceSize; ++i) {
    tree.Insert(sequence[i]);
  }
  PrintRbTree(tree);
  std::cout << std::endl;
}

void MoveInsert()
{
  std::cout << "<= MoveInsert =>" << std::endl;
  int sequence[] = {10, 5, 6, 11, 1, 9, 2};
  int sequenceSize = sizeof(sequence) / sizeof(int);
  Ds::RbTree<TestType> tree;
  for (int i = 0; i < sequenceSize; ++i) {
    TestType newValue(sequence[i], (float)sequence[i]);
    tree.Insert(Util::Move(newValue));
  }
  PrintRbTree(tree);
  std::cout << "ConstructorCount: " << TestType::smConstructorCount << std::endl
            << "MoveConstructorCount: " << TestType::smMoveConstructorCount
            << std::endl
            << std::endl;
  TestType::ResetCounts();
}

void Emplace()
{
  std::cout << "<= Emplace =>" << std::endl;
  int sequence[] = {0, 9, 1, 8, 2, 7, 3, 6, 4, 5};
  int sequenceSize = sizeof(sequence) / sizeof(int);
  Ds::RbTree<TestType> tree;
  for (int i = 0; i < sequenceSize; ++i) {
    tree.Emplace(sequence[i], (float)sequence[i]);
  }
  PrintRbTree(tree);
  std::cout << "ConstructorCount: " << TestType::smConstructorCount << std::endl
            << std::endl;
  TestType::ResetCounts();
}

void BasicRemove()
{
  std::cout << "<= BasicRemove =>" << std::endl;
  // This tests the basic cases for removal such as deleting the head or
  // removing dangling red nodes.
  Ds::RbTree<int> tree;
  tree.Insert(5);
  tree.Remove(5);
  PrintRbTree(tree);
  std::cout << "--- 0 ---" << std::endl;
  tree.Insert(5);
  tree.Insert(3);
  tree.Remove(3);
  PrintRbTree(tree);
  std::cout << "--- 1 ---" << std::endl;
  tree.Insert(7);
  tree.Remove(7);
  PrintRbTree(tree);
  std::cout << "--- 2 ---" << std::endl;
  tree.Insert(7);
  tree.Remove(5);
  PrintRbTree(tree);
  std::cout << "--- 3 ---" << std::endl;
  tree.Insert(5);
  tree.Remove(7);
  PrintRbTree(tree);
  std::cout << "--- 4 ---" << std::endl;
  tree.Insert(3);
  tree.Insert(7);
  tree.Remove(5);
  PrintRbTree(tree);
  std::cout << "--- 5 ---" << std::endl;
  tree.Insert(5);
  tree.Insert(4);
  tree.Remove(5);
  PrintRbTree(tree);
  std::cout << "--- 6 ---" << std::endl;
  tree.Insert(2);
  tree.Remove(4);
  PrintRbTree(tree);
  std::cout << "--- 7 ---" << std::endl;
  tree.Insert(10);
  tree.Insert(5);
  tree.Insert(6);
  tree.Insert(8);
  tree.Insert(15);
  tree.Insert(20);
  tree.Remove(10);
  PrintRbTree(tree);
  std::cout << "--- 8 ---" << std::endl;
  std::cout << std::endl;
}

void Iterator()
{
  std::cout << "<= Iterator =>" << std::endl;
  // Attempt to create an iterator for an empty tree.
  Ds::RbTree<int> tree;
  if (tree.cbegin() == tree.cend()) {
    std::cout << "Empty Tree" << std::endl;
  }
  std::cout << "--- 0 ---" << std::endl;

  // Insert random values into the tree.
  int insertionCount = 0;
  srand(10);
  while (insertionCount < 20) {
    int value = rand() % 100;
    if (!tree.Contains(value)) {
      tree.Insert(value);
      ++insertionCount;
    }
  }
  PrintRbTree(tree);
  std::cout << "--- 1 ---" << std::endl;

  // Iterate over the values in the tree.
  auto cIt = tree.cbegin();
  auto cItE = tree.cend();
  std::cout << *cIt;
  ++cIt;
  while (cIt != cItE) {
    std::cout << ", " << *cIt;
    ++cIt;
  }
  std::cout << std::endl << std::endl;
}

void ExtensiveModification()
{
  std::cout << "<= ExtensiveModification =>" << std::endl;
  Ds::RbTree<int> tree;
  constexpr int exchangeCount = 20;
  int treeValues[2][exchangeCount];
  int* front = treeValues[0];
  int* back = treeValues[1];

  auto checkTree = [&tree]()
  {
    LogAbortIf(
      !tree.HasConsistentBlackHeight(),
      "The black height of the tree is not consistent.");
    LogAbortIf(tree.HasDoubleRed(), "The tree has a double red.");
  };

  // This will insert values into the tree and store the inserted values in the
  // provided buffer.
  auto insertValues = [&checkTree, &tree, exchangeCount](int* buffer)
  {
    int insertionCount = 0;
    while (insertionCount < exchangeCount) {
      int value = rand() % 1000;
      if (!tree.Contains(value)) {
        tree.Insert(value);
        checkTree();
        buffer[insertionCount] = value;
        ++insertionCount;
      }
    }
  };

  // This will remove all of the values in the provided buffer from the tree.
  auto removeValues = [&checkTree, &tree, exchangeCount](int* buffer)
  {
    for (int i = 0; i < exchangeCount; ++i) {
      tree.Remove(buffer[i]);
      checkTree();
    }
  };

  // We begin by insterting values into the tree and the front buffer as an
  // initialization step.
  srand(5);
  insertValues(front);

  for (int i = 0; i < 10; ++i) {
    // Values are inserted into the tree and the back buffer first. After this,
    // all of the values in the front buffer are removed and the buffers are
    // swapped.
    insertValues(back);
    removeValues(front);
    PrintRbTree(tree);
    std::cout << "--- " << i << " ---" << std::endl;

    int* temp = front;
    front = back;
    back = temp;
  }

  // We end by emptying all values from the tree.
  removeValues(front);
  PrintRbTree(tree);
}

int main()
{
  EnableLeakOutput();
  LeftInsert();
  RightInsert();
  ExplicitInsert();
  MoveInsert();
  Emplace();
  BasicRemove();
  Iterator();
  ExtensiveModification();
}
