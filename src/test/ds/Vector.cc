#include <iostream>
#include <string>
#include <utility>

#include "debug/MemLeak.h"
#include "ds/Vector.h"
#include "test/ds/Print.h"
#include "test/ds/TestType.h"

void CopyConstructor0()
{
  std::cout << "<= CopyConstructor0 =>\n";
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

void CopyConstructor1()
{
  // This will ensure that copied vector elements are constructed and not merely
  // assigned to from the original vector.
  std::cout << "<= CopyConstructor1 =>\n";
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

void MoveConstructor()
{
  std::cout << "<= MoveConstructor =>\n";
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

void SinglePush()
{
  std::cout << "<= SinglePush =>\n";
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

void MovePush()
{
  std::cout << "<= MovePush =>\n";
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

void MultiplePush()
{
  std::cout << "<= MultiplePush =>\n";
  Ds::Vector<int> test;
  test.Push(0, 30);
  PrintVector(test);
  test.Push(1, 15);
  PrintVector(test);
}

void Emplace()
{
  std::cout << "<= Emplace =>\n";
  Ds::Vector<TestType> testVector;
  for (int i = 0; i < 15; ++i) {
    testVector.Emplace(i, (float)i);
  }
  PrintVector(testVector);
}

void Insert0()
{
  // Test insterting a new element at the end of the vector.
  std::cout << "<= Insert0 =>\n";
  Ds::Vector<TestType> test;
  for (int i = 0; i < Ds::Vector<TestType>::smStartCapacity; ++i) {
    test.Emplace(i);
  }
  test.Insert(Ds::Vector<int>::smStartCapacity, TestType(-1));
  PrintVector(test);
  TestType::PrintCounts();
}

void Insert1()
{
  // Test inserting new elments in the middle of the vector.
  std::cout << "<= Insert1 =>\n";
  Ds::Vector<TestType> test;
  for (int i = 0; i < Ds::Vector<TestType>::smStartCapacity; ++i) {
    test.Emplace(i);
  }
  for (int i = 0; i < 10; i += 2) {
    test.Insert(i, TestType(-1));
  }
  PrintVector(test);
  TestType::PrintCounts();
}

void Insert2()
{
  // Insert strings to ensure appropriate construction and destruction.
  std::cout << "<= Insert2 =>\n";
  Ds::Vector<std::string> test;
  test.Insert(0, "string 0");
  test.Insert(0, "string 1");
  test.Insert(0, "string 2");
  PrintVector(test);
}

void Pop()
{
  std::cout << "<= Pop =>\n";
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

void Clear()
{
  std::cout << "<= Clear =>\n";
  Ds::Vector<TestType> testVector;
  for (int i = 0; i < 5; ++i) {
    testVector.Emplace(i, (float)i);
  }

  testVector.Clear();
  PrintVector(testVector);
  TestType::PrintCounts();
}

void Remove0()
{
  std::cout << "<= Remove0 =>\n";
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

void Remove1()
{
  std::cout << "<= Remove1 =>\n";
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

void LazyRemove()
{
  std::cout << "<= LazyRemove =>\n";
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

void IndexOperator()
{
  std::cout << "<= IndexOperator =>\n";
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

void CopyAssignment0()
{
  // Test copying a vector to another vector with the same size.
  std::cout << "<= CopyAssignment0 =>\n";
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

void CopyAssignment1()
{
  // Test the case where the size of the vector being copied to is larger than
  // the size of the vector being copied from.
  std::cout << "<= CopyAssignment1 =>\n";
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

void CopyAssignment2()
{
  // Test the case where the vector being copied to has a smaller size than the
  // vector being copied from and the capacity of the vector being copied to is
  // greater than or equal to the size of the vector being copied from.
  std::cout << "<= CopyAssignment2 =>\n";
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

void CopyAssignment3()
{
  // Test the case where the vector being copied to has a capacity smaller than
  // the size of the vector being copied from.
  std::cout << "<= CopyAssignment3 =>\n";
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

void MoveAssignment()
{
  std::cout << "<= MoveAssignment =>\n";
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

void Find()
{
  std::cout << "<= Find =>\n";
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

void Contains()
{
  std::cout << "<= Contains =>\n";
  Ds::Vector<int> testVector;
  for (int i = 0; i < 20; ++i) {
    testVector.Push(i);
  }
  std::cout << "10: " << testVector.Contains(10) << '\n';
  std::cout << "21: " << testVector.Contains(21) << '\n';
}

void Resize0()
{
  // Test the case where the new size is smaller than the current size;
  std::cout << "<= Resize0 =>\n";
  Ds::Vector<TestType> test;
  for (int i = 0; i < 10; ++i) {
    test.Emplace(i);
  }
  test.Resize(5);
  PrintVector(test);
  TestType::PrintCounts();
}

void Resize1()
{
  // Test the case where the new size is larger than the current capacity.
  std::cout << "<= Resize1 =>\n";
  Ds::Vector<TestType> test;
  for (int i = 0; i < 10; ++i) {
    test.Emplace(i);
  }
  test.Resize(20, TestType(-1));
  PrintVector(test);
  TestType::PrintCounts();
}

void Resize2()
{
  // Ensure correct construction and destruction usage with std::string.
  std::cout << "<= Resize2 =>\n";
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

void Reserve()
{
  std::cout << "<= Reserve =>\n";
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

void Shrink0()
{
  // We perform two consecutive shrinks. The first makes the capacity and size
  // equal. Since capacity and size are equal, the second does nothing.
  std::cout << "<= Shrink0 =>\n";
  Ds::Vector<TestType> test;
  for (int i = 0; i < 5; ++i) {
    test.Emplace(i);
  }
  test.Shrink();
  test.Shrink();
  TestType::PrintCounts();
}

void Shrink1()
{
  // This will crash or cause memory leaks without the proper use of
  // constructors and destructors.
  std::cout << "<= Shrink1 =>\n";
  Ds::Vector<std::string> test;
  for (int i = 0; i < 5; ++i) {
    std::string newString;
    newString += (char)(i + 'A');
    test.Push(std::move(newString));
  }
  test.Shrink();
  PrintVector(test);
}

void CData()
{
  std::cout << "<= CData =>\n";
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

void Top()
{
  std::cout << "<= Top =>\n";
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

void InnerVector()
{
  std::cout << "<= InnerVector =>\n";
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

void BigInnerVector()
{
  std::cout << "<= BigInnerVector =>\n";
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

void ConstructionDestructionCounts()
{
  std::cout << "<= ConstructionDestructionCounts =>\n";
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

void Empty()
{
  // Copy an empty vector and make sure both vectors are empty.
  std::cout << "<= Empty =>\n";
  Ds::Vector<TestType> empty;
  Ds::Vector<TestType> notEmpty(empty);
  std::cout << "Empty: " << (empty.Empty() && notEmpty.Empty()) << '\n';
}

void RunTest(void (*test)())
{
  static bool firstTest = true;
  if (firstTest) {
    firstTest = false;
  }
  else {
    std::cout << '\n';
  }
  TestType::ResetCounts();
  test();
}

int main(void)
{
  EnableLeakOutput();
  RunTest(CopyConstructor0);
  RunTest(CopyConstructor1);
  RunTest(MoveConstructor);
  RunTest(SinglePush);
  RunTest(MovePush);
  RunTest(MultiplePush);
  RunTest(Emplace);
  RunTest(Insert0);
  RunTest(Insert1);
  RunTest(Insert2);
  RunTest(Pop);
  RunTest(Clear);
  RunTest(Remove0);
  RunTest(Remove1);
  RunTest(LazyRemove);
  RunTest(IndexOperator);
  RunTest(CopyAssignment0);
  RunTest(CopyAssignment1);
  RunTest(CopyAssignment2);
  RunTest(CopyAssignment3);
  RunTest(MoveAssignment);
  RunTest(Find);
  RunTest(Contains);
  RunTest(Resize0);
  RunTest(Resize1);
  RunTest(Resize2);
  RunTest(Reserve);
  RunTest(Shrink0);
  RunTest(Shrink1);
  RunTest(CData);
  RunTest(Top);
  RunTest(InnerVector);
  RunTest(BigInnerVector);
  RunTest(ConstructionDestructionCounts);
  RunTest(Empty);
}
