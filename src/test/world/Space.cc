#include <iostream>

#include "debug/MemLeak.h"
#include "test/world/Print.h"
#include "world/Space.h"

#pragma pack(push, 1)
struct Comp0
{
  static int smInitValue;
  float m0;
  float m1;

  Comp0(): m0((float)smInitValue), m1((float)smInitValue)
  {
    ++smInitValue;
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
int Comp0::smInitValue = 0;

struct Comp1
{
  static int smInitValue;
  double m0;
  double m1;

  Comp1(): m0((double)smInitValue), m1((double)smInitValue)
  {
    ++smInitValue;
  }
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
int Comp1::smInitValue = 1;

struct Comp2
{
  static int smInitValue;
  int m0;
  double m1;
  float m2;

  Comp2(): m0(smInitValue), m1((double)smInitValue), m2((float)smInitValue)
  {
    ++smInitValue;
  }
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
int Comp2::smInitValue = 2;

struct Comp3
{
  static int smInitValue;
  short m0;
  double m1;
  float m2;
  int m3;

  Comp3():
    m0((short)smInitValue),
    m1((double)smInitValue),
    m2((float)smInitValue),
    m3(smInitValue)
  {
    ++smInitValue;
  }
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
int Comp3::smInitValue = 3;
#pragma pack(pop)

template<typename T>
void PrintComponentData(const World::Space& space, int componentCount)
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

void CreateMember()
{
  std::cout << "<= CreateMember =>" << std::endl;
  World::Space space;
  for (int i = 0; i < 10; ++i)
  {
    space.CreateMember();
  }
  PrintSpace(space);
  std::cout << std::endl;
}

void DeleteMember()
{
  std::cout << "<= DeleteMember =>" << std::endl;
  World::Space space;
  World::MemberId memberIds[10];
  for (int i = 0; i < 10; ++i)
  {
    memberIds[i] = space.CreateMember();
  }
  std::cout << "Members Created" << std::endl;
  PrintSpaceMembers(space);
  PrintSpaceAddressBin(space);
  PrintSpaceUnusedMemberIds(space);

  for (int i = 0; i < 10; i += 2)
  {
    space.DeleteMember(memberIds[i]);
  }
  std::cout << "-----" << std::endl << "Members Removed" << std::endl;
  PrintSpaceMembers(space);
  PrintSpaceAddressBin(space);
  PrintSpaceUnusedMemberIds(space);
  std::cout << std::endl;
}

void AddComponent()
{
  std::cout << "<= AddComponent =>" << std::endl;
  World::Space space;
  World::MemberId mem0 = space.CreateMember();
  Comp0& mem0comp0 = space.AddComponent<Comp0>(mem0);
  Comp1& mem0comp1 = space.AddComponent<Comp1>(mem0);
  Comp2& mem0comp2 = space.AddComponent<Comp2>(mem0);
  World::MemberId mem1 = space.CreateMember();
  Comp0& mem1comp0 = space.AddComponent<Comp0>(mem1);
  Comp1& mem1comp1 = space.AddComponent<Comp1>(mem1);
  Comp2& mem1comp2 = space.AddComponent<Comp2>(mem1);
  World::MemberId mem2 = space.CreateMember();
  Comp0& mem2comp0 = space.AddComponent<Comp0>(mem2);
  Comp3& mem2comp3 = space.AddComponent<Comp3>(mem2);
  mem2comp3.SetData(5);

  PrintSpaceMembers(space);
  PrintSpaceAddressBin(space);

  Comp3& mem1comp3 = space.AddComponent<Comp3>(mem1);
  mem1comp3.SetData(4);
  Comp3& mem0comp3 = space.AddComponent<Comp3>(mem0);
  mem0comp3.SetData(3);
  Comp2& mem2comp2 = space.AddComponent<Comp2>(mem2);

  PrintSpace(space);
  PrintComponentData<Comp0>(space, 3);
  PrintComponentData<Comp1>(space, 2);
  PrintComponentData<Comp2>(space, 3);
  PrintComponentData<Comp3>(space, 3);
  std::cout << std::endl;
}

void RemComponent()
{
  std::cout << "<= RemComponent =>" << std::endl;
  World::Space space;
  World::MemberId mem0 = space.CreateMember();
  space.AddComponent<Comp0>(mem0);
  space.AddComponent<Comp1>(mem0);
  space.AddComponent<Comp2>(mem0);
  space.AddComponent<Comp3>(mem0);
  World::MemberId mem1 = space.CreateMember();
  space.AddComponent<Comp0>(mem1);
  space.AddComponent<Comp1>(mem1);
  space.AddComponent<Comp2>(mem1);
  World::MemberId mem2 = space.CreateMember();
  space.AddComponent<Comp1>(mem2);
  space.AddComponent<Comp3>(mem2);
  space.AddComponent<Comp2>(mem2);
  World::MemberId mem3 = space.CreateMember();
  space.AddComponent<Comp3>(mem3);
  space.AddComponent<Comp2>(mem3);
  space.AddComponent<Comp0>(mem3);

  PrintSpaceMembers(space);
  PrintSpaceAddressBin(space);

  space.RemComponent<Comp2>(mem2);
  space.RemComponent<Comp0>(mem1);
  space.RemComponent<Comp1>(mem2);
  space.RemComponent<Comp1>(mem0);

  PrintSpaceMembers(space);
  PrintSpaceAddressBin(space);

  space.AddComponent<Comp0>(mem2);

  PrintSpaceMembers(space);
  PrintSpaceAddressBin(space);
  PrintSpaceTables(space);
  std::cout << std::endl;
}

void DeleteMembersWithComponents()
{
  std::cout << "<= DeleteMembersWithComponents =>" << std::endl;
  World::Space space;
  World::MemberId memberIds[8];
  for (int i = 0; i < 8; ++i)
  {
    memberIds[i] = space.CreateMember();
    space.AddComponent<Comp0>(memberIds[i]);
    if (i % 2 == 0)
    {
      space.AddComponent<Comp1>(memberIds[i]);
    }
    if (i % 3 == 0)
    {
      space.AddComponent<Comp2>(memberIds[i]);
    }
    if (i % 5 == 0)
    {
      space.AddComponent<Comp3>(memberIds[i]);
    }
  }
  std::cout << "Members and Components Created" << std::endl;
  PrintSpaceMembers(space);
  PrintSpaceAddressBin(space);
  PrintSpaceUnusedMemberIds(space);
  PrintSpaceTablesOwners(space);

  space.DeleteMember(memberIds[0]);
  space.DeleteMember(memberIds[3]);
  space.DeleteMember(memberIds[4]);
  space.DeleteMember(memberIds[6]);
  space.DeleteMember(memberIds[7]);
  std::cout << "-----" << std::endl << "Members Removed" << std::endl;
  PrintSpaceMembers(space);
  PrintSpaceAddressBin(space);
  PrintSpaceUnusedMemberIds(space);
  PrintSpaceTablesOwners(space);

  World::MemberId newMemberId = space.CreateMember();
  space.AddComponent<Comp0>(newMemberId);
  space.AddComponent<Comp1>(newMemberId);
  space.AddComponent<Comp2>(newMemberId);
  newMemberId = space.CreateMember();
  space.AddComponent<Comp3>(newMemberId);
  space.AddComponent<Comp1>(newMemberId);
  std::cout << "-----" << std::endl
            << "New Members and Components" << std::endl;
  PrintSpaceMembers(space);
  PrintSpaceAddressBin(space);
  PrintSpaceUnusedMemberIds(space);
  PrintSpaceTablesOwners(space);
  std::cout << std::endl;
}

void GetComponent()
{
  std::cout << "<= GetComponent =>" << std::endl;
  World::Space space;
  World::MemberId mem0 = space.CreateMember();
  space.AddComponent<Comp0>(mem0);
  space.AddComponent<Comp1>(mem0);
  space.AddComponent<Comp2>(mem0);
  space.AddComponent<Comp3>(mem0);
  World::MemberId mem1 = space.CreateMember();
  space.AddComponent<Comp0>(mem1);
  space.AddComponent<Comp1>(mem1);
  space.AddComponent<Comp2>(mem1);

  Comp0* mem0comp0 = space.GetComponent<Comp0>(mem0);
  mem0comp0->SetData(0);
  Comp1* mem0comp1 = space.GetComponent<Comp1>(mem0);
  mem0comp1->SetData(1);
  Comp2* mem0comp2 = space.GetComponent<Comp2>(mem0);
  mem0comp2->SetData(2);
  Comp3* mem0comp3 = space.GetComponent<Comp3>(mem0);
  mem0comp3->SetData(3);
  Comp0* mem1comp0 = space.GetComponent<Comp0>(mem1);
  mem1comp0->SetData(1);
  Comp1* mem1comp1 = space.GetComponent<Comp1>(mem1);
  mem1comp1->SetData(2);
  Comp2* mem1comp2 = space.GetComponent<Comp2>(mem1);
  mem1comp2->SetData(3);

  PrintSpace(space);
  PrintComponentData<Comp0>(space, 2);
  PrintComponentData<Comp1>(space, 2);
  PrintComponentData<Comp2>(space, 2);
  PrintComponentData<Comp3>(space, 1);
  std::cout << std::endl;
}

void HasComponent()
{
  std::cout << "<= HasComponent =>" << std::endl;
  World::Space space;
  World::MemberId mem0 = space.CreateMember();
  space.AddComponent<Comp1>(mem0);
  space.AddComponent<Comp2>(mem0);
  World::MemberId mem1 = space.CreateMember();
  space.AddComponent<Comp0>(mem1);
  space.AddComponent<Comp3>(mem1);
  space.AddComponent<Comp3>(mem0);

  PrintSpaceMembers(space);
  PrintSpaceAddressBin(space);
  PrintSpaceTableLookup(space);
  std::cout << space.HasComponent<Comp0>(mem0);
  std::cout << space.HasComponent<Comp1>(mem0);
  std::cout << space.HasComponent<Comp2>(mem0);
  std::cout << space.HasComponent<Comp3>(mem0);
  std::cout << space.HasComponent<Comp0>(mem1);
  std::cout << space.HasComponent<Comp1>(mem1);
  std::cout << space.HasComponent<Comp2>(mem1);
  std::cout << space.HasComponent<Comp3>(mem1);
  std::cout << std::endl << std::endl;
}

int main(void)
{
  InitMemLeakOutput();
  CreateMember();
  DeleteMember();
  AddComponent();
  RemComponent();
  DeleteMembersWithComponents();
  GetComponent();
  HasComponent();
}
