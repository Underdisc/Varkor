#include <iostream>
#include <stdlib.h>
#include <string>
#include <utility>

#include "Error.h"
#include "debug/MemLeak.h"
#include "ds/RbTree.h"
#include "test/ds/Print.h"
#include "test/ds/Test.h"
#include "test/ds/TestType.h"

void LeftInsert() {
  // Every time a value is inserted, it will become the leftmost leaf node.
  Ds::RbTree<int> tree;
  for (int i = 20; i > 0; --i) {
    tree.Insert(i);
  }
  PrintRbTree(tree);
}

void RightInsert() {
  // Every time a value is inserted, it will become the rightmost leaf node.
  Ds::RbTree<int> tree;
  for (int i = 0; i < 20; ++i) {
    tree.Insert(i);
  }
  PrintRbTree(tree);
}

void ExplicitInsert() {
  // The insertions for this tree make use of every possible type of
  // transforation used when balancing the tree.
  int sequence[] = {18, 5,  10, 15, 16, 9,  6,  4,  2, 19, 20, 21,
                    22, 23, 24, 25, 26, 27, 28, 29, 1, 3,  0};
  int sequenceSize = sizeof(sequence) / sizeof(int);
  Ds::RbTree<int> tree;
  for (int i = 0; i < sequenceSize; ++i) {
    tree.Insert(sequence[i]);
  }
  PrintRbTree(tree);
}

void MoveInsert() {
  int sequence[] = {10, 5, 6, 11, 1, 9, 2};
  int sequenceSize = sizeof(sequence) / sizeof(int);
  Ds::RbTree<TestType> tree;
  for (int i = 0; i < sequenceSize; ++i) {
    TestType newValue(sequence[i], (float)sequence[i]);
    tree.Insert(std::move(newValue));
  }
  PrintRbTree(tree);
  TestType::PrintCounts();
}

void Emplace() {
  int sequence[] = {0, 9, 1, 8, 2, 7, 3, 6, 4, 5};
  int sequenceSize = sizeof(sequence) / sizeof(int);
  Ds::RbTree<TestType> tree;
  for (int i = 0; i < sequenceSize; ++i) {
    tree.Emplace(sequence[i], (float)sequence[i]);
  }
  PrintRbTree(tree);
  TestType::PrintCounts();
}

void BasicRemove() {
  // This tests the basic cases for removal such as deleting the head or
  // removing dangling red nodes.
  Ds::RbTree<int> tree;
  tree.Insert(5);
  tree.Remove(5);
  PrintRbTree(tree);
  std::cout << "--- 0 ---\n";
  tree.Insert(5);
  tree.Insert(3);
  tree.Remove(3);
  PrintRbTree(tree);
  std::cout << "--- 1 ---\n";
  tree.Insert(7);
  tree.Remove(7);
  PrintRbTree(tree);
  std::cout << "--- 2 ---\n";
  tree.Insert(7);
  tree.Remove(5);
  PrintRbTree(tree);
  std::cout << "--- 3 ---\n";
  tree.Insert(5);
  tree.Remove(7);
  PrintRbTree(tree);
  std::cout << "--- 4 ---\n";
  tree.Insert(3);
  tree.Insert(7);
  tree.Remove(5);
  PrintRbTree(tree);
  std::cout << "--- 5 ---\n";
  tree.Insert(5);
  tree.Insert(4);
  tree.Remove(5);
  PrintRbTree(tree);
  std::cout << "--- 6 ---\n";
  tree.Insert(2);
  tree.Remove(4);
  PrintRbTree(tree);
  std::cout << "--- 7 ---\n";
  tree.Insert(10);
  tree.Insert(5);
  tree.Insert(6);
  tree.Insert(8);
  tree.Insert(15);
  tree.Insert(20);
  tree.Remove(10);
  PrintRbTree(tree);
  std::cout << "--- 8 ---\n";
}

void Iterator() {
  // Attempt to create an iterator for an empty tree.
  Ds::RbTree<int> tree;
  if (tree.cbegin() == tree.cend()) {
    std::cout << "Empty Tree\n";
  }

  // Insert random values into the tree.
  std::cout << "--- 0 ---\n";
  constexpr int insertions = 20;
  int keys[insertions] = {71, 99, 72, 94, 97, 96, 22, 12, 26, 74,
                          41, 23, 4,  83, 62, 81, 29, 50, 59, 60};
  for (int i = 0; i < insertions; ++i) {
    tree.Insert(keys[i]);
  }
  PrintRbTree(tree);

  // Iterate over the values in the tree.
  std::cout << "--- 1 ---\n";
  auto cIt = tree.cbegin();
  auto cItE = tree.cend();
  std::cout << *cIt;
  ++cIt;
  while (cIt != cItE) {
    std::cout << ", " << *cIt;
    ++cIt;
  }
  std::cout << '\n';
}

void ExtensiveModification() {
  Ds::RbTree<int> tree;
  constexpr int valueArrays = 11;
  constexpr int exchangeCount = 20;
  int values[valueArrays][exchangeCount] = {
    {54,  693, 255, 449, 660, 430, 927, 649, 472, 640,
     114, 321, 533, 476, 426, 307, 963, 107, 150, 231},
    {517, 992, 193, 211, 936, 849, 352, 83,  922, 131,
     451, 969, 959, 110, 253, 615, 122, 487, 583, 234},
    {386, 6,   954, 585, 515, 519, 860, 415, 646, 612,
     684, 605, 891, 459, 633, 699, 70,  4,   17,  136},
    {888, 166, 238, 801, 610, 389, 590, 393, 151, 306,
     635, 950, 368, 46,  982, 275, 235, 482, 260, 453},
    {740, 609, 753, 285, 690, 616, 439, 793, 663, 585,
     86,  284, 317, 58,  78,  560, 919, 367, 385, 800},
    {326, 271, 980, 274, 278, 85,  543, 373, 833, 629,
     939, 56,  361, 875, 692, 683, 155, 686, 66,  370},
    {386, 821, 175, 746, 158, 872, 902, 751, 547, 182,
     216, 517, 391, 116, 168, 41,  425, 709, 986, 351},
    {533, 267, 589, 261, 946, 773, 382, 624, 891, 649,
     543, 654, 27,  332, 472, 676, 596, 816, 997, 497},
    {105, 573, 127, 58,  386, 452, 368, 407, 768, 559,
     761, 198, 2,   925, 886, 975, 16,  293, 989, 232},
    {782, 168, 274, 721, 771, 20,  167, 394, 858, 30,
     370, 300, 872, 24,  495, 408, 187, 536, 577, 171},
    {814, 426, 846, 508, 776, 402, 950, 555, 419, 424,
     946, 416, 34,  533, 460, 725, 558, 705, 193, 711}};

  auto checkTree = [&tree]() {
    LogAbortIf(
      !tree.HasConsistentBlackHeight(),
      "The black height of the tree is not consistent.");
    LogAbortIf(tree.HasDoubleRed(), "The tree has a double red.");
  };

  auto insertValues = [&checkTree, &tree, exchangeCount](const int* values) {
    for (int i = 0; i < exchangeCount; ++i) {
      tree.Insert(values[i]);
      checkTree();
    }
  };

  auto removeValues = [&checkTree, &tree, exchangeCount](const int* values) {
    for (int i = 0; i < exchangeCount; ++i) {
      tree.Remove(values[i]);
      checkTree();
    }
  };

  insertValues(values[0]);
  // Insert new values and remove previously inserted values on each iteration.
  for (int i = 0; i < 10; ++i) {
    insertValues(values[i + 1]);
    removeValues(values[i]);
    PrintRbTree(tree);
    std::cout << "--- " << i << " ---\n";
  }
  // Remove the remaining values.
  removeValues(values[10]);
  PrintRbTree(tree);
}

int main() {
  EnableLeakOutput();
  RunDsTest(LeftInsert);
  RunDsTest(RightInsert);
  RunDsTest(ExplicitInsert);
  RunDsTest(MoveInsert);
  RunDsTest(Emplace);
  RunDsTest(BasicRemove);
  RunDsTest(Iterator);
  RunDsTest(ExtensiveModification);
}
