#include <iostream>

#include "../dbg/mem_leak.h"
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

  DS::Vector<int> test_vector;
  std::cout << "<Empty>" << std::endl;
  PrintVector(test_vector);

  for (int i = 0; i < 5; ++i)
  {
    test_vector.Push(i);
  }
  std::cout << "<Pushed 5 Values>" << std::endl;
  PrintVector(test_vector);

  for (int i = 0; i < 20; ++i)
  {
    test_vector.Push(i);
  }
  std::cout << "<Pushed 20 Values>" << std::endl;
  PrintVector(test_vector);

  for (int i = 0; i < 10; ++i)
  {
    test_vector.Pop();
  }
  std::cout << "<Popped 10 Values>" << std::endl;
  PrintVector(test_vector);

  test_vector.Clear();
  std::cout << "<Cleared>" << std::endl;
  PrintVector(test_vector);
}

void IndexOperator()
{
  std::cout << "=IndexOperator=" << std::endl;

  DS::Vector<int> test_vector;
  for (int i = 0; i < 20; ++i)
  {
    test_vector.Push(i);
  }

  int val0 = test_vector[5];
  std::cout << "[5]: " << val0 << std::endl;
  const int& val1 = test_vector[10];
  std::cout << &val1 << std::endl;
  std::cout << test_vector._data + 10 << std::endl;
  std::cout << "[10]: " << val1 << std::endl;
  test_vector[15] = 7;
  std::cout << "[15]: " << test_vector[15] << std::endl;
}

int main(void)
{
  InitMemLeakOutput();
  PushPop();
  std::cout << std::endl;
  IndexOperator();
}
