#include <iostream>

#include "debug/MemLeak.h"
#include "ds/Vector.h"
#include "util/Utility.h"

template<typename T>
void PrintVector(const Ds::Vector<T>& vector, bool stats = true)
{
  if (stats)
  {
    std::cout << "Size: " << vector.Size() << std::endl;
    std::cout << "Capactiy: " << vector.Capacity() << std::endl;
  }
  if (vector.Size() == 0)
  {
    std::cout << "[]" << std::endl;
    return;
  }
  std::cout << "[";
  for (int i = 0; i < vector.Size() - 1; ++i)
  {
    std::cout << vector[i] << ", ";
  }
  std::cout << vector[vector.Size() - 1] << "]" << std::endl;
}

void CopyConstructor()
{
  std::cout << "<= CopyConstructor =>" << std::endl;
  const int elementCount = 15;
  Ds::Vector<int> testVector;
  for (int i = 0; i < elementCount; ++i)
  {
    testVector.Push(elementCount - i);
  }
  Ds::Vector<int> copyVector(testVector);
  PrintVector(testVector);
  PrintVector(copyVector);
  std::cout << std::endl;
}

void MoveConstructor()
{
  std::cout << "<= MoveConstructor =>" << std::endl;
  Ds::Vector<int> ogVector;
  for (int i = 0; i < 20; ++i)
  {
    ogVector.Push(i);
  }
  const int* ogVectorPtr = ogVector.CData();
  Ds::Vector<int> moveVector(Util::Move(ogVector));
  const int* moveVectorPtr = moveVector.CData();
  PrintVector(ogVector);
  PrintVector(moveVector);
  std::cout << "Moved: " << (ogVectorPtr == moveVectorPtr) << std::endl;
  std::cout << std::endl;
}

void SinglePush()
{
  std::cout << "<= SinglePush =>" << std::endl;
  Ds::Vector<int> testVector;
  PrintVector(testVector);
  for (int i = 0; i < 5; ++i)
  {
    testVector.Push(i);
  }
  PrintVector(testVector);
  for (int i = 0; i < 20; ++i)
  {
    testVector.Push(i);
  }
  PrintVector(testVector);
  std::cout << std::endl;
}

void MovePush()
{
  std::cout << "<= MovePush =>" << std::endl;
  // We create a vector of vectors and save the pointers to the data for each of
  // the inner vectors.
  Ds::Vector<Ds::Vector<int>> testVector;
  const int innerVectorCount = 6;
  const int* ogInnerPtrs[innerVectorCount];
  for (int i = 0; i < innerVectorCount; ++i)
  {
    Ds::Vector<int> innerVector;
    for (int j = 0; j < 10; ++j)
    {
      innerVector.Push(i + j);
    }
    ogInnerPtrs[i] = innerVector.CData();
    testVector.Push(Util::Move(innerVector));
  }

  // We print out the content of each inner vector and we also make sure that
  // pointers for the inner vectors are still the same as the pointers in use
  // when the inner vectors were first created.
  const int* movedInnerPtrs[innerVectorCount];
  for (int i = 0; i < innerVectorCount; ++i)
  {
    movedInnerPtrs[i] = testVector[i].CData();
    PrintVector(testVector[i], false);
  }
  std::cout << "Moved: ";
  for (int i = 0; i < innerVectorCount; ++i)
  {
    std::cout << (ogInnerPtrs[i] == movedInnerPtrs[i]);
  }
  std::cout << std::endl << std::endl;
}

void MultiplePush()
{
  std::cout << "<= MultiplePush =>" << std::endl;
  Ds::Vector<int> test;
  test.Push(0, 30);
  PrintVector(test);
  test.Push(1, 15);
  PrintVector(test);
  std::cout << std::endl;
}

struct TestType
{
  int mA;
  float mB;
  TestType()
  {
    ++smDefaultConstructorCount;
  }
  TestType(const TestType& other): mA(other.mA), mB(other.mB)
  {
    ++smCopyConstructorCount;
  }
  TestType(int a, float b): mA(a), mB(b)
  {
    ++smConstructorCount;
  }
  ~TestType()
  {
    ++smDestructorCount;
  }

  static int smDefaultConstructorCount;
  static int smCopyConstructorCount;
  static int smConstructorCount;
  static int smDestructorCount;

  static void ResetCounts()
  {
    smDefaultConstructorCount = 0;
    smCopyConstructorCount = 0;
    smConstructorCount = 0;
    smDestructorCount = 0;
  }
};
int TestType::smDefaultConstructorCount = 0;
int TestType::smCopyConstructorCount = 0;
int TestType::smConstructorCount = 0;
int TestType::smDestructorCount = 0;

std::ostream& operator<<(std::ostream& os, const TestType& rhs)
{
  os << "[" << rhs.mA << ", " << rhs.mB << "]";
  return os;
}

void Emplace()
{
  std::cout << "<= Emplace =>" << std::endl;
  Ds::Vector<TestType> testVector;
  for (int i = 0; i < 15; ++i)
  {
    testVector.Emplace(i, (float)i);
  }
  PrintVector(testVector);
  std::cout << std::endl;
}

void Pop()
{
  std::cout << "<= Pop =>" << std::endl;
  Ds::Vector<TestType> testVector;
  for (int i = 0; i < 10; ++i)
  {
    testVector.Emplace(i, (float)i);
  }

  TestType::ResetCounts();
  for (int i = 0; i < 5; ++i)
  {
    testVector.Pop();
  }
  PrintVector(testVector);
  std::cout << "DestructorCount: " << TestType::smDestructorCount << std::endl
            << std::endl;
}

void Clear()
{
  std::cout << "<= Clear =>" << std::endl;
  Ds::Vector<TestType> testVector;
  for (int i = 0; i < 5; ++i)
  {
    testVector.Emplace(i, (float)i);
  }

  TestType::ResetCounts();
  testVector.Clear();
  PrintVector(testVector);
  std::cout << "DestructorCount: " << TestType::smDestructorCount << std::endl
            << std::endl;
}

void IndexOperator()
{
  std::cout << "<= IndexOperator =>" << std::endl;
  Ds::Vector<int> testVector;
  for (int i = 0; i < 20; ++i)
  {
    testVector.Push(i);
  }

  int val0 = testVector[5];
  std::cout << "[5]: " << val0 << std::endl;
  const int& val1 = testVector[10];
  std::cout << "[10]: " << val1 << std::endl;
  testVector[15] = 7;
  std::cout << "[15]: " << testVector[15] << std::endl;
  std::cout << std::endl;
}

void CopyAssignment()
{
  std::cout << "<= CopyAssignment =>" << std::endl;
  // We first test the case where the vector being copied from has a size larger
  // than the capacity of the vector being copied to.
  Ds::Vector<int> ogVector;
  for (int i = 0; i < 15; ++i)
  {
    ogVector.Push(i);
  }
  // We push one value into the copy vector so it allocates some memory. That
  // way we can make sure the vector is freeing its old pointer.
  Ds::Vector<int> copyVector;
  copyVector.Push(1);
  copyVector = ogVector;
  PrintVector(ogVector);
  PrintVector(copyVector);

  // We then test the case where the vector being copied to already has enough
  // space to copy all of the elements from the other vector.
  ogVector.Clear();
  for (int i = 0; i < 7; ++i)
  {
    ogVector.Push(i);
  }
  copyVector = ogVector;
  PrintVector(ogVector);
  PrintVector(copyVector);
  std::cout << std::endl;
}

void MoveAssignment()
{
  std::cout << "<= MoveAssignment =>" << std::endl;
  // This test is nearly identical to the MoveConstructor test, but instead of
  // using the move constructor, we use the move assignment operator.
  Ds::Vector<int> ogVector;
  for (int i = 0; i < 20; ++i)
  {
    ogVector.Push(i);
  }
  const int* ogVectorPtr = ogVector.CData();
  Ds::Vector<int> moveVector;
  moveVector = Util::Move(ogVector);
  const int* moveVectorPtr = moveVector.CData();
  PrintVector(ogVector);
  PrintVector(moveVector);
  std::cout << "Moved: " << (ogVectorPtr == moveVectorPtr) << std::endl;
  std::cout << std::endl;
}

void Contains()
{
  std::cout << "<= Contains =>" << std::endl;
  Ds::Vector<int> testVector;
  for (int i = 0; i < 20; ++i)
  {
    testVector.Push(i);
  }
  std::cout << "10: " << testVector.Contains(10) << std::endl;
  std::cout << "21: " << testVector.Contains(21) << std::endl;
  std::cout << std::endl;
}

void Resize()
{
  std::cout << "<= Resize =>" << std::endl;
  Ds::Vector<int> test;
  test.Resize(20, 0);
  PrintVector(test);
  test.Resize(30, 1);
  PrintVector(test);
  test.Resize(15, 2);
  PrintVector(test);
  std::cout << std::endl;
}

void Reserve()
{
  std::cout << "<= Reserve =>" << std::endl;
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
  for (int i = 0; i < 10; ++i)
  {
    test1.Push(i);
  }
  test1.Reserve(30);
  PrintVector(test1);
  std::cout << std::endl;
}

void CData()
{
  std::cout << "<= CData =>" << std::endl;
  Ds::Vector<int> test;
  for (int i = 0; i < 5; ++i)
  {
    test.Push(i);
  }
  const int* data = test.CData();
  std::cout << "[";
  for (int i = 0; i < 4; ++i)
  {
    std::cout << data[i] << ", ";
  }
  std::cout << data[4] << "]" << std::endl;
  std::cout << std::endl;
}

void Top()
{
  std::cout << "<= Top =>" << std::endl;
  Ds::Vector<int> test;
  for (int i = 0; i < 10; ++i)
  {
    test.Push(i * 2);
  }
  std::cout << test.Top() << std::endl;
  for (int i = 0; i < 5; ++i)
  {
    test.Pop();
  }
  std::cout << test.Top() << std::endl;
  test.Pop();
  test.Top() = 20;
  std::cout << test.Top() << std::endl;
  std::cout << std::endl;
}

void InnerVector()
{
  std::cout << "<= InnerVector =>" << std::endl;
  Ds::Vector<Ds::Vector<TestType>> testVector;

  for (int i = 0; i < 5; ++i)
  {
    Ds::Vector<TestType> innerTest;
    for (int j = 0; j < 5; ++j)
    {
      int value = i + j;
      innerTest.Push(TestType(value, (float)value));
    }
    testVector.Push(innerTest);
  }

  for (int i = 0; i < 5; ++i)
  {
    PrintVector(testVector[i], false);
  }
  std::cout << std::endl;
}

void BigInnerVector()
{
  std::cout << "<= BigInnerVector =>" << std::endl;
  // We create one vector that will contain 100 Vectors and each of those will
  // have 100 TestType values. Besides being a minor stress test, this also
  // ensures that the outer vector grows without crashing or losing any of the
  // data in the subvectors.
  Ds::Vector<Ds::Vector<TestType>> testVector;
  for (int i = 0; i < 100; ++i)
  {
    Ds::Vector<TestType> innerTest;
    for (int j = 0; j < 100; ++j)
    {
      int value = i + j;
      innerTest.Push(TestType(value, (float)value));
    }
    testVector.Push(Util::Move(innerTest));
  }

  std::cout << "Main Vector Stats: [" << testVector.Size() << ", "
            << testVector.Capacity() << "]" << std::endl
            << "[0][0]: " << testVector[0][0] << std::endl
            << "[25][25]: " << testVector[25][25] << std::endl
            << "[50][50]: " << testVector[50][50] << std::endl
            << "[75][75]: " << testVector[75][75] << std::endl
            << "[99][99]: " << testVector[99][99] << std::endl
            << std::endl;
}

void ConstructionDestructionCounts()
{
  std::cout << "<= ConstructionDestructionCounts =>" << std::endl;
  // We reset the counts on TestType and perform a good chunk of vector
  // operations to see how many times constructors and destructors are called.
  TestType::ResetCounts();
  Ds::Vector<Ds::Vector<TestType>> testVector;
  for (int i = 0; i < 30; ++i)
  {
    Ds::Vector<TestType> inner;
    for (int j = 0; j < 100; ++j)
    {
      int value = i + j;
      inner.Emplace(value, (float)value);
    }
    testVector.Push(Util::Move(inner));
  }
  testVector.Clear();

  std::cout << "DefaultConstructorCount: "
            << TestType::smDefaultConstructorCount << std::endl
            << "CopyConstructorCount: " << TestType::smCopyConstructorCount
            << std::endl
            << "ConstructorCount: " << TestType::smConstructorCount << std::endl
            << "DestructorCount: " << TestType::smDestructorCount << std::endl;
}

int main(void)
{
  InitMemLeakOutput();
  CopyConstructor();
  MoveConstructor();
  SinglePush();
  MovePush();
  MultiplePush();
  Emplace();
  Pop();
  Clear();
  IndexOperator();
  CopyAssignment();
  MoveAssignment();
  Contains();
  Resize();
  Reserve();
  CData();
  Top();
  InnerVector();
  BigInnerVector();
  ConstructionDestructionCounts();
}
