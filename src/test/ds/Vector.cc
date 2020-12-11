#include <iostream>

#include "debug/MemLeak.h"
#include "ds/Vector.h"

template<typename T>
void PrintVector(const Ds::Vector<T>& vector)
{
  std::cout << "Size: " << vector.Size() << std::endl;
  std::cout << "Capactiy: " << vector.Capacity() << std::endl;
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
  TestType() {}
  TestType(int a, float b): mA(a), mB(b) {}
  ~TestType()
  {
    ++smDestructorCount;
  }

  static int smDestructorCount;
  static void ResetDestructorCount()
  {
    smDestructorCount = 0;
  }
};
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

  TestType::ResetDestructorCount();
  for (int i = 0; i < 5; ++i)
  {
    testVector.Pop();
  }
  PrintVector(testVector);
  std::cout << "Destructor Calls: " << TestType::smDestructorCount << std::endl
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

  TestType::ResetDestructorCount();
  testVector.Clear();
  PrintVector(testVector);
  std::cout << "Destructor Calls: " << TestType::smDestructorCount << std::endl
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
}

int main(void)
{
  InitMemLeakOutput();
  SinglePush();
  MultiplePush();
  Emplace();
  Pop();
  Clear();
  IndexOperator();
  Contains();
  Resize();
  Top();
}
