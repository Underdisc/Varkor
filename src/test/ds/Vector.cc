#include <iostream>
#include <string>
#include <utility>

#include "debug/MemLeak.h"
#include "ds/Vector.h"
#include "test/ds/Print.h"
#include "test/ds/Test.h"
#include "test/ds/TestType.h"

void CopyConstructor0() {
  Ds::Vector<TestType> test;
  for (int i = 0; i < 15; ++i) {
    test.Emplace(15 - i);
  }
  Ds::Vector<TestType> copy(test);
  std::cout << "<- test ->\n";
  PrintVector(test);
  std::cout << "<- copy ->\n";
  PrintVector(copy);
  TestType::PrintCounts();
}

void CopyConstructor1() {
  // This will ensure that copied vector elements are constructed and not merely
  // assigned to from the original vector.
  Ds::Vector<std::string> test;
  for (int i = 0; i < 10; ++i) {
    std::string newElement;
    newElement += (char)('A' + i);
    test.Push(newElement);
  }
  Ds::Vector<std::string> copy(test);
  std::cout << "<- test ->\n";
  PrintVector(test);
  std::cout << "<- copy ->\n";
  PrintVector(copy);
}

void InitializerList() {
  Ds::Vector<std::string> test = {"a", "b", "c", "d", "e"};
  PrintVector(test);
  test = {"f", "g", "h", "i", "j"};
  PrintVector(test);
}

void MoveConstructor() {
  Ds::Vector<int> ogVector;
  for (int i = 0; i < 20; ++i) {
    ogVector.Push(i);
  }
  const int* ogVectorPtr = ogVector.CData();
  Ds::Vector<int> moveVector(std::move(ogVector));
  const int* moveVectorPtr = moveVector.CData();
  PrintVector(ogVector);
  PrintVector(moveVector);
  std::cout << "Moved: " << (ogVectorPtr == moveVectorPtr) << '\n';
}

void SinglePush() {
  Ds::Vector<int> testVector;
  PrintVector(testVector);
  for (int i = 0; i < 5; ++i) {
    testVector.Push(i);
  }
  PrintVector(testVector);
  for (int i = 0; i < 20; ++i) {
    testVector.Push(i);
  }
  PrintVector(testVector);
}

void MovePush() {
  // We create a vector of vectors and save the pointers to the data for each of
  // the inner vectors.
  Ds::Vector<Ds::Vector<int>> testVector;
  const int innerVectorCount = 6;
  const int* ogInnerPtrs[innerVectorCount];
  for (int i = 0; i < innerVectorCount; ++i) {
    Ds::Vector<int> innerVector;
    for (int j = 0; j < 10; ++j) {
      innerVector.Push(i + j);
    }
    ogInnerPtrs[i] = innerVector.CData();
    testVector.Push(std::move(innerVector));
  }

  // We print out the content of each inner vector and we also make sure that
  // pointers for the inner vectors are still the same as the pointers in use
  // when the inner vectors were first created.
  const int* movedInnerPtrs[innerVectorCount];
  for (int i = 0; i < innerVectorCount; ++i) {
    movedInnerPtrs[i] = testVector[i].CData();
    PrintVector(testVector[i], false);
  }
  std::cout << "Moved: ";
  for (int i = 0; i < innerVectorCount; ++i) {
    std::cout << (ogInnerPtrs[i] == movedInnerPtrs[i]);
  }
  std::cout << '\n';
}

void MultiplePush() {
  Ds::Vector<int> test;
  test.Push(0, 30);
  PrintVector(test);
  test.Push(1, 15);
  PrintVector(test);
}

void Emplace() {
  Ds::Vector<TestType> testVector;
  for (int i = 0; i < 15; ++i) {
    testVector.Emplace(i, (float)i);
  }
  PrintVector(testVector);
}

void Insert0() {
  // Test insterting a new element at the end of the vector.
  Ds::Vector<TestType> test;
  for (int i = 0; i < Ds::Vector<TestType>::smStartCapacity; ++i) {
    test.Emplace(i);
  }
  TestType newElement(-1);
  test.Insert(Ds::Vector<int>::smStartCapacity, newElement);
  PrintVector(test);
  TestType::PrintCounts();
}

void Insert1() {
  // Test inserting new elments in the middle of the vector.
  Ds::Vector<TestType> test;
  for (int i = 0; i < Ds::Vector<TestType>::smStartCapacity; ++i) {
    test.Emplace(i);
  }
  TestType newElement(-1);
  for (int i = 0; i < 10; i += 2) {
    test.Insert(i, newElement);
  }
  PrintVector(test);
  TestType::PrintCounts();
}

void Insert2() {
  // Insert strings to ensure appropriate construction and destruction.
  Ds::Vector<std::string> test;
  test.Insert(0, "string 0");
  test.Insert(0, "string 1");
  test.Insert(0, "string 2");
  PrintVector(test);
}

void Insert3() {
  // Test move insertion.
  Ds::Vector<TestType> test;
  test.Reserve(10);
  for (int i = 0; i < 5; ++i) {
    test.Emplace(-1);
  }
  for (int i = 5; i > 0; i--) {
    test.Insert(i, TestType(-1));
  }
  PrintVector(test);
  TestType::PrintCounts();
}

void Swap() {
  Ds::Vector<TestType> test;
  for (int i = 0; i < 5; ++i) {
    test.Emplace(i);
  }
  test.Swap(2, 4);
  test.Swap(0, 2);
  PrintVector(test);
  TestType::PrintCounts();
}

void Sort() {
  // Initialize vectors to be sorted.
  Ds::Vector<int> test[4];
  int array[2][10] = {
    {8, 6, 9, 0, 1, 3, 7, 5, 4, 2}, {4, 4, 3, 3, 2, 2, 1, 1, 0, 0}};
  for (int i = 0; i < 10; ++i) {
    test[0].Push(9 - i);
    test[1].Push(array[0][i]);
    test[2].Push(array[1][i]);
  }
  srand(34);
  for (int i = 0; i < 100'000; ++i) {
    test[3].Push(rand());
  }

  // Sort the vectors and make sure the result is truly sorted.
  auto ensureSort = [](const Ds::Vector<int>& vector) {
    for (int j = 0; j < vector.Size() - 1; ++j) {
      LogAbortIf(vector[j] > vector[j + 1], "Elements not sorted");
    }
  };
  for (int i = 0; i < 4; ++i) {
    test[i].Sort();
    ensureSort(test[i]);
  }
  std::cout << "Sorts Successful" << std::endl;
}

void Pop() {
  Ds::Vector<TestType> testVector;
  for (int i = 0; i < 10; ++i) {
    testVector.Emplace(i, (float)i);
  }

  for (int i = 0; i < 5; ++i) {
    testVector.Pop();
  }
  PrintVector(testVector);
  TestType::PrintCounts();
}

void Clear() {
  Ds::Vector<TestType> testVector;
  for (int i = 0; i < 5; ++i) {
    testVector.Emplace(i, (float)i);
  }

  testVector.Clear();
  PrintVector(testVector);
  TestType::PrintCounts();
}

void Remove0() {
  Ds::Vector<TestType> testVector;
  for (int i = 0; i < 10; ++i) {
    testVector.Emplace(i, (float)i);
  }
  PrintVector(testVector, false);
  for (int i = 8; i >= 0; i -= 2) {
    testVector.Remove(i);
  }
  PrintVector(testVector, false);
  for (int i = 4; i >= 0; --i) {
    testVector.Remove(i);
  }
  PrintVector(testVector, false);
  TestType::PrintCounts();
}

void Remove1() {
  Ds::Vector<std::string> test;
  for (int i = 0; i < 5; ++i) {
    std::string newElement;
    newElement += (char)('A' + i);
    test.Push(newElement);
  }
  test.Remove(4);
  test.Remove(1);
  test.Remove(0);
  PrintVector(test);
}

void LazyRemove0() {
  Ds::Vector<TestType> testVector;
  for (int i = 0; i < 10; ++i) {
    testVector.Emplace(i, (float)i);
  }

  for (int i = 8; i >= 0; i -= 2) {
    testVector.LazyRemove(i);
  }
  PrintVector(testVector, false);
  for (int i = 0; i < 5; ++i) {
    testVector.LazyRemove(0);
  }
  PrintVector(testVector, false);
  TestType::PrintCounts();
}

void LazyRemove1() {
  Ds::Vector<std::string> testVector;
  for (int i = 0; i < 10; ++i) {
    testVector.Push(std::to_string(i));
  }
  for (int i = 0; i < 5; ++i) {
    testVector.LazyRemove(i);
  }
  PrintVector(testVector, false);
}

void IndexOperator() {
  Ds::Vector<int> testVector;
  for (int i = 0; i < 20; ++i) {
    testVector.Push(i);
  }

  int val0 = testVector[5];
  std::cout << "[5]: " << val0 << '\n';
  const int& val1 = testVector[10];
  std::cout << "[10]: " << val1 << '\n';
  testVector[15] = 7;
  std::cout << "[15]: " << testVector[15] << '\n';
}

void CopyAssignment0() {
  // Test copying a vector to another vector with the same size.
  Ds::Vector<TestType> test, copy;
  for (int i = 0; i < 5; ++i) {
    test.Emplace(i);
    copy.Emplace(5 - i);
  }
  copy = test;
  std::cout << "<- test ->\n";
  PrintVector(test);
  std::cout << "<- copy ->\n";
  PrintVector(copy);
  TestType::PrintCounts();
}

void CopyAssignment1() {
  // Test the case where the size of the vector being copied to is larger than
  // the size of the vector being copied from.
  Ds::Vector<TestType> test, copy;
  for (int i = 0; i < 5; ++i) {
    test.Emplace(i);
  }
  for (int i = 0; i < 8; ++i) {
    copy.Emplace(i);
  }
  copy = test;
  std::cout << "<- test ->\n";
  PrintVector(test);
  std::cout << "<- copy ->\n";
  PrintVector(copy);
  TestType::PrintCounts();
}

void CopyAssignment2() {
  // Test the case where the vector being copied to has a smaller size than the
  // vector being copied from and the capacity of the vector being copied to is
  // greater than or equal to the size of the vector being copied from.
  Ds::Vector<TestType> test, copy;
  for (int i = 0; i < 7; ++i) {
    test.Emplace(i);
  }
  for (int i = 0; i < 4; ++i) {
    copy.Emplace(i);
  }
  copy = test;
  std::cout << "<- test ->\n";
  PrintVector(test);
  std::cout << "<- copy ->\n";
  PrintVector(copy);
  TestType::PrintCounts();
}

void CopyAssignment3() {
  // Test the case where the vector being copied to has a capacity smaller than
  // the size of the vector being copied from.
  Ds::Vector<TestType> test, copy;
  for (int i = 0; i < 15; ++i) {
    test.Emplace(i);
  }
  copy.Emplace(0);
  copy = test;
  std::cout << "<- test ->\n";
  PrintVector(test);
  std::cout << "<- copy ->\n";
  PrintVector(copy);
  TestType::PrintCounts();
}

void MoveAssignment() {
  // We push values into the vector being moved from and the vector being moved
  // to to ensure that the data in the vector being moved to is deallocated.
  Ds::Vector<int> ogVector;
  for (int i = 0; i < 20; ++i) {
    ogVector.Push(i);
  }
  Ds::Vector<int> moveVector;
  for (int i = 20; i > 0; --i) {
    moveVector.Push(i);
  }
  const int* ogVectorPtr = ogVector.CData();
  moveVector = std::move(ogVector);
  const int* moveVectorPtr = moveVector.CData();
  PrintVector(ogVector);
  PrintVector(moveVector);
  std::cout << "Moved: " << (ogVectorPtr == moveVectorPtr) << '\n';
}

void Find() {
  Ds::Vector<int> test;
  for (int i = 0; i < 20; ++i) {
    if (i % 2 == 0) {
      test.Push(i);
    }
  }
  std::cout << "4: " << test.Find(4) << '\n';
  std::cout << "12: " << test.Find(12) << '\n';
  std::cout << "13: " << test.Find(13) << '\n';
}

void Contains() {
  Ds::Vector<int> testVector;
  for (int i = 0; i < 20; ++i) {
    testVector.Push(i);
  }
  std::cout << "10: " << testVector.Contains(10) << '\n';
  std::cout << "21: " << testVector.Contains(21) << '\n';
}

void Resize0() {
  // Test the case where the new size is smaller than the current size;
  Ds::Vector<TestType> test;
  for (int i = 0; i < 10; ++i) {
    test.Emplace(i);
  }
  test.Resize(5);
  PrintVector(test);
  TestType::PrintCounts();
}

void Resize1() {
  // Test the case where the new size is larger than the current capacity.
  Ds::Vector<TestType> test;
  for (int i = 0; i < 10; ++i) {
    test.Emplace(i);
  }
  test.Resize(20, TestType(-1));
  PrintVector(test);
  TestType::PrintCounts();
}

void Resize2() {
  // Ensure correct construction and destruction usage with std::string.
  Ds::Vector<std::string> test;
  for (int i = 0; i < 10; ++i) {
    std::string newElement;
    newElement += (char)('A' + i);
    test.Push(newElement);
  }
  test.Resize(20, "a");
  test.Resize(10);
  PrintVector(test);
}

void Reserve() {
  // We first test the case where we give an empty vector some capacity and then
  // we make sure reserving a smaller capacity doesn't change the capacity of
  // the vector.
  Ds::Vector<int> test0;
  test0.Reserve(50);
  PrintVector(test0);
  test0.Reserve(25);
  PrintVector(test0);

  // We then test reserving on a vector that already has values in it and will
  // need a new allocation for the reservation.
  Ds::Vector<int> test1;
  for (int i = 0; i < 10; ++i) {
    test1.Push(i);
  }
  test1.Reserve(30);
  PrintVector(test1);
}

void Shrink0() {
  // We perform two consecutive shrinks. The first makes the capacity and size
  // equal. Since capacity and size are equal, the second does nothing.
  Ds::Vector<TestType> test;
  for (int i = 0; i < 5; ++i) {
    test.Emplace(i);
  }
  test.Shrink();
  test.Shrink();
  TestType::PrintCounts();
}

void Shrink1() {
  // This will crash or cause memory leaks without the proper use of
  // constructors and destructors.
  Ds::Vector<std::string> test;
  for (int i = 0; i < 5; ++i) {
    std::string newString;
    newString += (char)(i + 'A');
    test.Push(std::move(newString));
  }
  test.Shrink();
  PrintVector(test);
}

void CData() {
  Ds::Vector<int> test;
  for (int i = 0; i < 5; ++i) {
    test.Push(i);
  }
  const int* data = test.CData();
  std::cout << "[";
  for (int i = 0; i < 4; ++i) {
    std::cout << data[i] << ", ";
  }
  std::cout << data[4] << "]\n";
}

void Top() {
  Ds::Vector<int> test;
  for (int i = 0; i < 10; ++i) {
    test.Push(i * 2);
  }
  std::cout << test.Top() << '\n';
  for (int i = 0; i < 5; ++i) {
    test.Pop();
  }
  std::cout << test.Top() << '\n';
  test.Pop();
  test.Top() = 20;
  std::cout << test.Top() << '\n';
}

void InnerVector() {
  Ds::Vector<Ds::Vector<TestType>> testVector;

  for (int i = 0; i < 5; ++i) {
    Ds::Vector<TestType> innerTest;
    for (int j = 0; j < 5; ++j) {
      int value = i + j;
      innerTest.Push(TestType(value, (float)value));
    }
    testVector.Push(innerTest);
  }

  for (int i = 0; i < 5; ++i) {
    PrintVector(testVector[i], false);
  }
}

void BigInnerVector() {
  // We create one vector that will contain 100 Vectors and each of those will
  // have 100 TestType values. Besides being a minor stress test, this also
  // ensures that the outer vector grows without crashing or losing any of the
  // data in the subvectors.
  Ds::Vector<Ds::Vector<TestType>> testVector;
  for (int i = 0; i < 100; ++i) {
    Ds::Vector<TestType> innerTest;
    for (int j = 0; j < 100; ++j) {
      int value = i + j;
      innerTest.Push(TestType(value, (float)value));
    }
    testVector.Push(std::move(innerTest));
  }

  std::cout << "Main Vector Stats: [" << testVector.Size() << ", "
            << testVector.Capacity() << "]\n"
            << "[0][0]: " << testVector[0][0] << '\n'
            << "[25][25]: " << testVector[25][25] << '\n'
            << "[50][50]: " << testVector[50][50] << '\n'
            << "[75][75]: " << testVector[75][75] << '\n'
            << "[99][99]: " << testVector[99][99] << '\n';
}

void ConstructionDestructionCounts() {
  // We reset the counts on TestType and perform a good chunk of vector
  // operations to see how many times constructors and destructors are called.
  Ds::Vector<Ds::Vector<TestType>> testVector;
  for (int i = 0; i < 30; ++i) {
    Ds::Vector<TestType> inner;
    for (int j = 0; j < 100; ++j) {
      int value = i + j;
      inner.Emplace(value, (float)value);
    }
    testVector.Push(std::move(inner));
  }
  testVector.Clear();
  TestType::PrintCounts();
}

void Empty() {
  // Copy an empty vector and make sure both vectors are empty.
  Ds::Vector<TestType> empty;
  Ds::Vector<TestType> notEmpty(empty);
  std::cout << "Empty: " << (empty.Empty() && notEmpty.Empty()) << '\n';
}

int main(void) {
  Error::Init();
  EnableLeakOutput();
  RunDsTest(CopyConstructor0);
  RunDsTest(CopyConstructor1);
  RunDsTest(MoveConstructor);
  RunDsTest(InitializerList);
  RunDsTest(SinglePush);
  RunDsTest(MovePush);
  RunDsTest(MultiplePush);
  RunDsTest(Emplace);
  RunDsTest(Insert0);
  RunDsTest(Insert1);
  RunDsTest(Insert2);
  RunDsTest(Insert3);
  RunDsTest(Swap);
  RunDsTest(Sort);
  RunDsTest(Pop);
  RunDsTest(Clear);
  RunDsTest(Remove0);
  RunDsTest(Remove1);
  RunDsTest(LazyRemove0);
  RunDsTest(LazyRemove1);
  RunDsTest(IndexOperator);
  RunDsTest(CopyAssignment0);
  RunDsTest(CopyAssignment1);
  RunDsTest(CopyAssignment2);
  RunDsTest(CopyAssignment3);
  RunDsTest(MoveAssignment);
  RunDsTest(Find);
  RunDsTest(Contains);
  RunDsTest(Resize0);
  RunDsTest(Resize1);
  RunDsTest(Resize2);
  RunDsTest(Reserve);
  RunDsTest(Shrink0);
  RunDsTest(Shrink1);
  RunDsTest(CData);
  RunDsTest(Top);
  RunDsTest(InnerVector);
  RunDsTest(BigInnerVector);
  RunDsTest(ConstructionDestructionCounts);
  RunDsTest(Empty);
}
