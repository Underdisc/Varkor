#include <iostream>

#include "core/ObjectSpace.h"
#include "debug/MemLeak.h"

#pragma pack(push, 1)
struct Comp0
{
  float m0;
  float m1;
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

struct Comp1
{
  double m0;
  double m1;
  void SetData(int value)
  {
    m0 = (double)value;
    m1 = (double)value;
  }
  void PrintData() const
  {
    std::cout << "[" << m0 << ", " << m1 << "]";
  }
};

struct Comp2
{
  int m0;
  double m1;
  float m2;
  void SetData(int value)
  {
    m0 = value;
    m1 = (double)value;
    m2 = (float)value;
  }
  void PrintData() const
  {
    std::cout << "[" << m0 << ", " << m1 << ", " << m2 << "]";
  }
};

struct Comp3
{
  short m0;
  double m1;
  float m2;
  int m3;
  void SetData(int value)
  {
    m0 = (short)value;
    m1 = (double)value;
    m2 = (float)value;
    m3 = value;
  }
  void PrintData() const
  {
    std::cout << "[" << m0 << ", " << m1 << ", " << m2 << ", " << m3 << "]";
  }
};
#pragma pack(pop)

template<typename T>
void PrintComponentData(const Core::ObjectSpace& space, int componentCount)
{
  const T* compData = space.GetComponentData<T>();
  compData[0].PrintData();
  for (int i = 1; i < componentCount; ++i)
  {
    std::cout << ", ";
    compData[i].PrintData();
  }
  std::cout << std::endl;
}

void CreateObject()
{
  std::cout << "-=CreateObject=-" << std::endl;
  Core::ObjectSpace space;
  for (int i = 0; i < 10; ++i)
  {
    space.CreateObject();
  }
  space.ShowAll();
  std::cout << std::endl;
}

void AddComponent()
{
  std::cout << "-=AddComponent=-" << std::endl;
  Core::ObjectSpace space;
  ObjRef obj0 = space.CreateObject();
  Comp0& obj0comp0 = space.AddComponent<Comp0>(obj0);
  obj0comp0.SetData(0);
  Comp1& obj0comp1 = space.AddComponent<Comp1>(obj0);
  obj0comp1.SetData(1);
  Comp2& obj0comp2 = space.AddComponent<Comp2>(obj0);
  obj0comp2.SetData(2);
  ObjRef obj1 = space.CreateObject();
  Comp0& obj1comp0 = space.AddComponent<Comp0>(obj1);
  obj1comp0.SetData(1);
  Comp1& obj1comp1 = space.AddComponent<Comp1>(obj1);
  obj1comp1.SetData(2);
  Comp2& obj1comp2 = space.AddComponent<Comp2>(obj1);
  obj1comp2.SetData(3);
  ObjRef obj2 = space.CreateObject();
  Comp0& obj2comp0 = space.AddComponent<Comp0>(obj2);
  obj2comp0.SetData(2);
  Comp3& obj2comp3 = space.AddComponent<Comp3>(obj2);
  obj2comp3.SetData(5);

  space.ShowObjects();
  space.ShowAddressBin();

  Comp3& obj1comp3 = space.AddComponent<Comp3>(obj1);
  obj1comp3.SetData(4);
  Comp3& obj0comp3 = space.AddComponent<Comp3>(obj0);
  obj0comp3.SetData(3);
  Comp2& obj2comp2 = space.AddComponent<Comp2>(obj2);
  obj2comp2.SetData(4);

  space.ShowAll();
  PrintComponentData<Comp0>(space, 3);
  PrintComponentData<Comp1>(space, 2);
  PrintComponentData<Comp2>(space, 3);
  PrintComponentData<Comp3>(space, 3);
  std::cout << std::endl;
}

void RemComponent()
{
  std::cout << "-=RemComponent=-" << std::endl;
  Core::ObjectSpace space;
  ObjRef obj0 = space.CreateObject();
  space.AddComponent<Comp0>(obj0);
  space.AddComponent<Comp1>(obj0);
  space.AddComponent<Comp2>(obj0);
  space.AddComponent<Comp3>(obj0);
  ObjRef obj1 = space.CreateObject();
  space.AddComponent<Comp0>(obj1);
  space.AddComponent<Comp1>(obj1);
  space.AddComponent<Comp2>(obj1);
  ObjRef obj2 = space.CreateObject();
  space.AddComponent<Comp1>(obj2);
  space.AddComponent<Comp3>(obj2);
  space.AddComponent<Comp2>(obj2);
  ObjRef obj3 = space.CreateObject();
  space.AddComponent<Comp3>(obj3);
  space.AddComponent<Comp2>(obj3);
  space.AddComponent<Comp0>(obj3);

  space.ShowObjects();
  space.ShowAddressBin();

  space.RemComponent<Comp2>(obj2);
  space.RemComponent<Comp0>(obj1);
  space.RemComponent<Comp1>(obj2);
  space.RemComponent<Comp1>(obj0);

  space.ShowObjects();
  space.ShowAddressBin();

  space.AddComponent<Comp0>(obj2);

  space.ShowObjects();
  space.ShowAddressBin();
  space.ShowTables();
  std::cout << std::endl;
}

void GetComponent()
{
  std::cout << "-=GetComponent=-" << std::endl;
  Core::ObjectSpace space;
  ObjRef obj0 = space.CreateObject();
  space.AddComponent<Comp0>(obj0);
  space.AddComponent<Comp1>(obj0);
  space.AddComponent<Comp2>(obj0);
  space.AddComponent<Comp3>(obj0);
  ObjRef obj1 = space.CreateObject();
  space.AddComponent<Comp0>(obj1);
  space.AddComponent<Comp1>(obj1);
  space.AddComponent<Comp2>(obj1);

  Comp0& obj0comp0 = space.GetComponent<Comp0>(obj0);
  obj0comp0.SetData(0);
  Comp1& obj0comp1 = space.GetComponent<Comp1>(obj0);
  obj0comp1.SetData(1);
  Comp2& obj0comp2 = space.GetComponent<Comp2>(obj0);
  obj0comp2.SetData(2);
  Comp3& obj0comp3 = space.GetComponent<Comp3>(obj0);
  obj0comp3.SetData(3);
  Comp0& obj1comp0 = space.GetComponent<Comp0>(obj1);
  obj1comp0.SetData(1);
  Comp1& obj1comp1 = space.GetComponent<Comp1>(obj1);
  obj1comp1.SetData(2);
  Comp2& obj1comp2 = space.GetComponent<Comp2>(obj1);
  obj1comp2.SetData(3);

  space.ShowAll();
  PrintComponentData<Comp0>(space, 2);
  PrintComponentData<Comp1>(space, 2);
  PrintComponentData<Comp2>(space, 2);
  PrintComponentData<Comp3>(space, 1);
  std::cout << std::endl;
}

void HasComponent()
{
  std::cout << "-=HasComponent=-" << std::endl;
  Core::ObjectSpace space;
  ObjRef obj0 = space.CreateObject();
  space.AddComponent<Comp1>(obj0);
  space.AddComponent<Comp2>(obj0);
  ObjRef obj1 = space.CreateObject();
  space.AddComponent<Comp0>(obj1);
  space.AddComponent<Comp3>(obj1);
  space.AddComponent<Comp3>(obj0);

  space.ShowObjects();
  space.ShowAddressBin();
  space.ShowTableLookup();
  std::cout << space.HasComponent<Comp0>(obj0);
  std::cout << space.HasComponent<Comp1>(obj0);
  std::cout << space.HasComponent<Comp2>(obj0);
  std::cout << space.HasComponent<Comp3>(obj0);
  std::cout << space.HasComponent<Comp0>(obj1);
  std::cout << space.HasComponent<Comp1>(obj1);
  std::cout << space.HasComponent<Comp2>(obj1);
  std::cout << space.HasComponent<Comp3>(obj1);
  std::cout << std::endl << std::endl;
}

int main(void)
{
  InitMemLeakOutput();
  CreateObject();
  AddComponent();
  RemComponent();
  GetComponent();
  HasComponent();
}
