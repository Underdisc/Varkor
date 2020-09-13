#include <iostream>

#include "../debug/mem_leak.h"
#include "../ds/vector.hh"

void PrintVector(const DS::Vector<int>& vector)
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

void PushPop()
{
  std::cout << "=PushPop=" << std::endl;

  DS::Vector<int> testVector;
  std::cout << "<Empty>" << std::endl;
  PrintVector(testVector);

  for (int i = 0; i < 5; ++i)
  {
    testVector.Push(i);
  }
  std::cout << "<Pushed 5 Values>" << std::endl;
  PrintVector(testVector);

  for (int i = 0; i < 20; ++i)
  {
    testVector.Push(i);
  }
  std::cout << "<Pushed 20 Values>" << std::endl;
  PrintVector(testVector);

  for (int i = 0; i < 10; ++i)
  {
    testVector.Pop();
  }
  std::cout << "<Popped 10 Values>" << std::endl;
  PrintVector(testVector);

  testVector.Clear();
  std::cout << "<Cleared>" << std::endl;
  PrintVector(testVector);
}

void IndexOperator()
{
  std::cout << "=IndexOperator=" << std::endl;

  DS::Vector<int> testVector;
  for (int i = 0; i < 20; ++i)
  {
    testVector.Push(i);
  }

  int val0 = testVector[5];
  std::cout << "[5]: " << val0 << std::endl;
  const int& val1 = testVector[10];
  std::cout << &val1 << std::endl;
  std::cout << testVector.mData + 10 << std::endl;
  std::cout << "[10]: " << val1 << std::endl;
  testVector[15] = 7;
  std::cout << "[15]: " << testVector[15] << std::endl;
}

void Contains()
{
  std::cout << "=Contains=" << std::endl;

  DS::Vector<int> testVector;
  for (int i = 0; i < 20; ++i)
  {
    testVector.Push(i);
  }
  std::cout << "Contains 10: " << testVector.Contains(10) << std::endl;
  std::cout << "Contains 21: " << testVector.Contains(21) << std::endl;
}

int main(void)
{
  InitMemLeakOutput();
  PushPop();
  std::cout << std::endl;
  IndexOperator();
  std::cout << std::endl;
  Contains();
}
