#include <iostream>

#include "comp/Type.h"
#include "ds/Vector.h"
#include "test/ds/Print.h"

#pragma pack(push, 1)
struct Simple0
{
  float m0;
  float m1;

  Simple0()
  {
    SetData(Comp::Type<Simple0>::smId);
  }
  Simple0(int value)
  {
    SetData(value);
  }
  void SetData(int value)
  {
    m0 = (float)value;
    m1 = (float)value;
  }
  void PrintData() const
  {
    std::cout << "[" << m0 << ", " << m1 << "]";
  }
};

struct Simple1
{
  double m0;
  int m1;

  Simple1()
  {
    SetData(Comp::Type<Simple1>::smId);
  }
  Simple1(int value)
  {
    SetData(value);
  }
  void SetData(int value)
  {
    m0 = (double)value;
    m1 = value;
  }
  void PrintData() const
  {
    std::cout << "[" << m0 << ", " << m1 << "]";
  }
};

struct Dynamic
{
  int* m0;
  double m1;
  float m2;

  Dynamic(): m0(nullptr)
  {
    SetData(Comp::Type<Dynamic>::smId);
  }
  Dynamic(int value): m0(nullptr)
  {
    SetData(value);
  }
  Dynamic(const Dynamic& other): m0(nullptr)
  {
    SetData(*other.m0);
  }
  ~Dynamic()
  {
    delete m0;
  }
  void SetData(int value)
  {
    if (m0 == nullptr)
    {
      m0 = alloc int;
    }
    *m0 = value;
    m1 = (double)value;
    m2 = (float)value;
  }
  void PrintData() const
  {
    std::cout << "[" << *m0 << ", " << m1 << ", " << m2 << "]";
  }
};

struct Container
{
  Ds::Vector<int> mVector;

  Container()
  {
    SetData(Comp::Type<Container>::smId);
  }
  Container(int value)
  {
    SetData(value);
  }
  void SetData(int value)
  {
    for (int i = 0; i < value; ++i)
    {
      mVector.Push(value + i);
    }
  }
  void PrintData() const
  {
    std::cout << mVector;
  }
};
#pragma pack(pop)

void RegisterComponentTypes()
{
  Comp::Type<Simple0>::Register();
  Comp::Type<Simple1>::Register();
  Comp::Type<Dynamic>::Register();
  Comp::Type<Container>::Register();
}
