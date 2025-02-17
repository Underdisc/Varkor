#ifndef test_world_TestTypes_h
#define test_world_TestTypes_h

#include <iostream>

#include "comp/Relationship.h"
#include "comp/Type.h"
#include "ds/Vector.h"
#include "test/Test.h"
#include "test/ds/Print.h"
#include "vlk/Valkor.h"
#include "world/Registrar.h"

#pragma pack(push, 1)
struct CallCounter
{
  CallCounter()
  {
    ++smDefaultConstructorCount;
  }
  CallCounter(const CallCounter& other)
  {
    ++smCopyConstructorCount;
  }
  CallCounter(CallCounter&& other)
  {
    ++smMoveConstructorCount;
  }
  CallCounter& operator=(CallCounter&& other)
  {
    ++smMoveAssignmentCount;
    mData = other.mData;
    return *this;
  }
  ~CallCounter()
  {
    ++smDestructorCount;
  }
  static void Reset()
  {
    smDefaultConstructorCount = 0;
    smCopyConstructorCount = 0;
    smMoveConstructorCount = 0;
    smMoveAssignmentCount = 0;
    smDestructorCount = 0;
  }
  static void Print()
  {
    std::cout << "-Call Counts-"
              << "\nDefault Constructor Count: " << smDefaultConstructorCount
              << "\nCopy Constructor Count: " << smCopyConstructorCount
              << "\nMove Constructor Count: " << smMoveConstructorCount
              << "\nMove Assignment Count: " << smMoveAssignmentCount
              << "\nDestructor Count: " << smDestructorCount << '\n';
  }
  int mData;
  static int smDefaultConstructorCount;
  static int smCopyConstructorCount;
  static int smMoveConstructorCount;
  static int smMoveAssignmentCount;
  static int smDestructorCount;
};
int CallCounter::smDefaultConstructorCount;
int CallCounter::smCopyConstructorCount;
int CallCounter::smMoveConstructorCount;
int CallCounter::smMoveAssignmentCount;
int CallCounter::smDestructorCount;

#define RunCallCounterTest(function) \
  TestHeader(function);              \
  function();                        \
  CallCounter::Print();              \
  CallCounter::Reset();              \
  std::cout << std::endl

struct Simple0
{
  float m0;
  float m1;
  Simple0()
  {
    SetData(Comp::Type<Simple0>::smId);
  }
  void SetData(int value)
  {
    m0 = (float)value;
    m1 = (float)value;
  }
  void VSerialize(Vlk::Value& val)
  {
    val("m0") = m0;
    val("m1") = m1;
  }
};

std::ostream& operator<<(std::ostream& os, const Simple0& comp)
{
  os << "[" << comp.m0 << ", " << comp.m1 << "]";
  return os;
}

struct Simple1
{
  double m0;
  int m1;
  Simple1()
  {
    SetData(Comp::Type<Simple1>::smId);
  }
  void SetData(int value)
  {
    m0 = (double)value;
    m1 = value;
  }
  void VSerialize(Vlk::Value& val)
  {
    val("m0") = m0;
    val("m1") = m1;
  }
};

std::ostream& operator<<(std::ostream& os, const Simple1& comp)
{
  os << "[" << comp.m0 << ", " << comp.m1 << "]";
  return os;
}

struct Dynamic
{
  int* m0;
  double m1;
  float m2;
  Dynamic(): m0(nullptr)
  {
    SetData(Comp::Type<Dynamic>::smId);
  }
  Dynamic(const Dynamic& other): m0(nullptr)
  {
    SetData(*other.m0);
  }
  Dynamic(Dynamic&& other): m0(other.m0), m1(other.m1), m2(other.m2)
  {
    other.m0 = nullptr;
    other.m1 = 0.0;
    other.m2 = 0.0f;
  }
  Dynamic& operator=(Dynamic&& other)
  {
    delete m0;
    m0 = other.m0;
    m1 = other.m1;
    m2 = other.m2;
    other.m0 = nullptr;
    other.m1 = 0.0;
    other.m2 = 0.0f;
    return *this;
  }
  ~Dynamic()
  {
    if (m0 != nullptr) {
      delete m0;
    }
  }
  void SetData(int value)
  {
    if (m0 == nullptr) {
      m0 = alloc int;
    }
    *m0 = value;
    m1 = (double)value;
    m2 = (float)value;
  }
  void VSerialize(Vlk::Value& val)
  {
    val("m0") = *m0;
    val("m1") = m1;
    val("m2") = m2;
  }
};

std::ostream& operator<<(std::ostream& os, const Dynamic& comp)
{
  os << "[" << *comp.m0 << ", " << comp.m1 << ", " << comp.m2 << "]";
  return os;
}

struct Container
{
  Ds::Vector<int> mVector;
  Container()
  {
    SetData(Comp::Type<Container>::smId);
  }
  void SetData(int value)
  {
    mVector.Clear();
    for (int i = 0; i < value / 2; ++i) {
      mVector.Push(value + i);
    }
  }
  void VSerialize(Vlk::Value& val)
  {
    val("m0") = mVector;
  }
};

std::ostream& operator<<(std::ostream& os, const Container& comp)
{
  os << comp.mVector;
  return os;
}

struct Dependant
{
  int m0;
  Dependant()
  {
    SetData(Comp::Type<Dependant>::smId);
  }
  void SetData(int value)
  {
    m0 = value;
  }
  void VSerialize(Vlk::Value& val)
  {
    val("m0") = m0;
  }
};

std::ostream& operator<<(std::ostream& os, const Dependant& comp)
{
  os << "[" << comp.m0 << "]";
  return os;
}

#pragma pack(pop)

void RegisterComponentTypes()
{
  RegisterComponent(CallCounter);
  RegisterComponent(Simple0);
  RegisterComponent(Simple1);
  RegisterComponent(Dynamic);
  RegisterComponent(Container);
  RegisterComponent(Dependant);
  RegisterDependencies(Dependant, CallCounter, Dynamic);
  RegisterComponent(Comp::Relationship);
}

#endif
