#include <iostream>

#include "comp/Type.h"
#include "debug/MemLeak.h"
#include "test/world/Print.h"
#include "test/world/TestTypes.h"
#include "world/Space.h"
#include "world/Types.h"

template<typename T>
void PrintComponentData(const World::Space& space)
{
  std::cout << Comp::Type<T>::smId << ": ";
  space.VisitTableComponents<T>(
    [](World::MemberId memberId, T& component)
    {
      component.PrintData();
    });
  std::cout << std::endl;
}

void CreateMember()
{
  std::cout << "<= CreateMember =>" << std::endl;
  World::Space space;
  for (size_t i = 0; i < 20; ++i)
  {
    space.CreateMember();
  }
  PrintSpaceMembers(space);
  std::cout << std::endl;
}

void DeleteMember()
{
  std::cout << "<= DeleteMember =>" << std::endl;
  World::Space space;
  World::MemberId memberIds[10];
  for (size_t i = 0; i < 10; ++i)
  {
    memberIds[i] = space.CreateMember();
  }

  for (size_t i = 0; i < 10; i += 2)
  {
    space.DeleteMember(memberIds[i]);
  }
  PrintSpaceMembers(space);
  PrintSpaceUnusedMemberIds(space);
  std::cout << std::endl;
}

void ParentChildMembers()
{
  std::cout << "<= ParentChildMembers =>" << std::endl;
  World::Space space;
  World::MemberId memberIds[10];
  for (size_t i = 0; i < 10; ++i)
  {
    memberIds[i] = space.CreateMember();
  }
  space.MakeParent(memberIds[0], memberIds[1]);
  space.MakeParent(memberIds[0], memberIds[2]);
  space.MakeParent(memberIds[0], memberIds[3]);
  space.MakeParent(memberIds[1], memberIds[4]);
  space.MakeParent(memberIds[2], memberIds[5]);
  space.MakeParent(memberIds[3], memberIds[6]);
  space.MakeParent(memberIds[3], memberIds[7]);
  space.CreateChildMember(memberIds[0]);
  space.CreateChildMember(memberIds[1]);
  space.CreateChildMember(memberIds[2]);
  space.CreateChildMember(memberIds[3]);
  PrintSpaceMembers(space);

  // Delete the member with children and create members that take the MemberIds
  // that were once used by the children.
  space.DeleteMember(memberIds[0]);
  for (size_t i = 0; i < 11; ++i)
  {
    space.CreateMember();
  }
  PrintSpaceMembers(space);
  std::cout << std::endl;
}

void AddComponent()
{
  std::cout << "<= AddComponent =>" << std::endl;
  World::Space space;
  World::MemberId mem0 = space.CreateMember();
  space.AddComponent<Simple0>(mem0);
  space.AddComponent<Simple1>(mem0);
  space.AddComponent<Dynamic>(mem0);
  World::MemberId mem1 = space.CreateMember();
  space.AddComponent<Simple0>(mem1);
  space.AddComponent<Simple1>(mem1);
  space.AddComponent<Dynamic>(mem1);
  World::MemberId mem2 = space.CreateMember();
  space.AddComponent<Simple0>(mem2);
  space.AddComponent<Container>(mem2, 5);

  PrintSpaceMembers(space);
  PrintSpaceDescriptorBin(space);

  space.AddComponent<Container>(mem1, 4);
  space.AddComponent<Container>(mem0, 3);
  space.AddComponent<Dynamic>(mem2);

  PrintSpace(space);
  PrintComponentData<Simple0>(space);
  PrintComponentData<Simple1>(space);
  PrintComponentData<Dynamic>(space);
  PrintComponentData<Container>(space);
  std::cout << std::endl;
}

void RemComponent()
{
  std::cout << "<= RemComponent =>" << std::endl;
  World::Space space;
  World::MemberId mem0 = space.CreateMember();
  space.AddComponent<Simple0>(mem0);
  space.AddComponent<Simple1>(mem0);
  space.AddComponent<Dynamic>(mem0);
  space.AddComponent<Container>(mem0);
  World::MemberId mem1 = space.CreateMember();
  space.AddComponent<Simple0>(mem1);
  space.AddComponent<Simple1>(mem1);
  space.AddComponent<Dynamic>(mem1);
  World::MemberId mem2 = space.CreateMember();
  space.AddComponent<Simple1>(mem2);
  space.AddComponent<Container>(mem2);
  space.AddComponent<Dynamic>(mem2);
  World::MemberId mem3 = space.CreateMember();
  space.AddComponent<Container>(mem3);
  space.AddComponent<Dynamic>(mem3);
  space.AddComponent<Simple0>(mem3);

  PrintSpaceMembers(space);
  PrintSpaceDescriptorBin(space);

  space.RemComponent<Dynamic>(mem2);
  space.RemComponent<Simple0>(mem1);
  space.RemComponent<Simple1>(mem2);
  space.RemComponent<Simple1>(mem0);

  PrintSpaceMembers(space);
  PrintSpaceDescriptorBin(space);

  space.AddComponent<Simple0>(mem2);

  PrintSpaceMembers(space);
  PrintSpaceDescriptorBin(space);
  PrintSpaceTables(space);
  std::cout << std::endl;
}

void DeleteMembersWithComponents()
{
  std::cout << "<= DeleteMembersWithComponents =>" << std::endl;
  World::Space space;
  World::MemberId memberIds[8];
  for (size_t i = 0; i < 8; ++i)
  {
    memberIds[i] = space.CreateMember();
    space.AddComponent<Simple0>(memberIds[i]);
    if (i % 2 == 0)
    {
      space.AddComponent<Simple1>(memberIds[i]);
    }
    if (i % 3 == 0)
    {
      space.AddComponent<Dynamic>(memberIds[i]);
    }
    if (i % 5 == 0)
    {
      space.AddComponent<Container>(memberIds[i]);
    }
  }
  std::cout << "Members and Components Created" << std::endl;
  PrintSpaceMembers(space);
  PrintSpaceDescriptorBin(space);
  PrintSpaceUnusedMemberIds(space);
  PrintSpaceTablesOwners(space);

  space.DeleteMember(memberIds[0]);
  space.DeleteMember(memberIds[3]);
  space.DeleteMember(memberIds[4]);
  space.DeleteMember(memberIds[6]);
  space.DeleteMember(memberIds[7]);
  std::cout << "-----" << std::endl << "Members Removed" << std::endl;
  PrintSpaceMembers(space);
  PrintSpaceDescriptorBin(space);
  PrintSpaceUnusedMemberIds(space);
  PrintSpaceTablesOwners(space);

  World::MemberId newMemberId = space.CreateMember();
  space.AddComponent<Simple0>(newMemberId);
  space.AddComponent<Simple1>(newMemberId);
  space.AddComponent<Dynamic>(newMemberId);
  newMemberId = space.CreateMember();
  space.AddComponent<Container>(newMemberId);
  space.AddComponent<Simple1>(newMemberId);
  std::cout << "-----" << std::endl
            << "New Members and Components" << std::endl;
  PrintSpaceMembers(space);
  PrintSpaceDescriptorBin(space);
  PrintSpaceUnusedMemberIds(space);
  PrintSpaceTablesOwners(space);
  std::cout << std::endl;
}

void GetComponent()
{
  std::cout << "<= GetComponent =>" << std::endl;
  World::Space space;
  World::MemberId mem0 = space.CreateMember();
  space.AddComponent<Simple0>(mem0);
  space.AddComponent<Simple1>(mem0);
  space.AddComponent<Dynamic>(mem0);
  space.AddComponent<Container>(mem0);
  World::MemberId mem1 = space.CreateMember();
  space.AddComponent<Simple0>(mem1);
  space.AddComponent<Simple1>(mem1);
  space.AddComponent<Dynamic>(mem1);

  Simple0* mem0comp0 = space.GetComponent<Simple0>(mem0);
  mem0comp0->SetData(0);
  Simple1* mem0comp1 = space.GetComponent<Simple1>(mem0);
  mem0comp1->SetData(1);
  Dynamic* mem0comp2 = space.GetComponent<Dynamic>(mem0);
  mem0comp2->SetData(2);
  Container* mem0comp3 = space.GetComponent<Container>(mem0);
  mem0comp3->SetData(3);
  Simple0* mem1comp0 = space.GetComponent<Simple0>(mem1);
  mem1comp0->SetData(1);
  Simple1* mem1comp1 = space.GetComponent<Simple1>(mem1);
  mem1comp1->SetData(2);
  Dynamic* mem1comp2 = space.GetComponent<Dynamic>(mem1);
  mem1comp2->SetData(3);

  PrintSpace(space);
  PrintComponentData<Simple0>(space);
  PrintComponentData<Simple1>(space);
  PrintComponentData<Dynamic>(space);
  PrintComponentData<Container>(space);
  std::cout << std::endl;
}

void HasComponent()
{
  std::cout << "<= HasComponent =>" << std::endl;
  World::Space space;
  World::MemberId mem0 = space.CreateMember();
  space.AddComponent<Simple1>(mem0);
  space.AddComponent<Dynamic>(mem0);
  World::MemberId mem1 = space.CreateMember();
  space.AddComponent<Simple0>(mem1);
  space.AddComponent<Container>(mem1);
  space.AddComponent<Container>(mem0);

  PrintSpaceMembers(space);
  PrintSpaceDescriptorBin(space);
  std::cout << space.HasComponent<Simple0>(mem0)
            << space.HasComponent<Simple1>(mem0)
            << space.HasComponent<Dynamic>(mem0)
            << space.HasComponent<Container>(mem0)
            << space.HasComponent<Simple0>(mem1)
            << space.HasComponent<Simple1>(mem1)
            << space.HasComponent<Dynamic>(mem1)
            << space.HasComponent<Container>(mem1)
            << space.HasComponent<Simple0>(World::nInvalidMemberId) << std::endl
            << std::endl;
}

void Duplicate()
{
  std::cout << "<= Duplicate =>" << std::endl;
  World::Space space;

  // A lone member.
  World::MemberId testId = space.CreateMember();
  space.AddComponent<Simple0>(testId, 0);
  space.AddComponent<Simple1>(testId, 0);
  space.Duplicate(testId);

  // A member with children.
  testId = space.CreateMember();
  space.AddComponent<Simple0>(testId, 1);
  space.AddComponent<Simple1>(testId, 1);
  World::MemberId childId = space.CreateChildMember(testId);
  space.AddComponent<Dynamic>(childId, 1);
  space.AddComponent<Container>(childId, 1);
  childId = space.CreateChildMember(testId);
  space.AddComponent<Dynamic>(childId, 1);
  space.Duplicate(testId);

  // A member with a parent and children.
  World::MemberId parentId = space.CreateMember();
  testId = space.CreateChildMember(parentId);
  space.AddComponent<Simple0>(testId, 2);
  space.AddComponent<Dynamic>(testId, 2);
  childId = space.CreateChildMember(testId);
  space.AddComponent<Simple0>(childId, 2);
  space.AddComponent<Container>(childId, 2);
  childId = space.CreateChildMember(testId);
  space.AddComponent<Simple1>(childId, 2);
  space.AddComponent<Container>(childId, 2);
  space.Duplicate(testId);

  PrintSpaceMembers(space);
  std::cout << "-Tables-" << std::endl;
  PrintComponentData<Simple0>(space);
  PrintComponentData<Simple1>(space);
  PrintComponentData<Dynamic>(space);
  PrintComponentData<Container>(space);
}

int main(void)
{
  InitMemLeakOutput();
  RegisterComponentTypes();

  CreateMember();
  DeleteMember();
  ParentChildMembers();
  AddComponent();
  RemComponent();
  DeleteMembersWithComponents();
  GetComponent();
  HasComponent();
  Duplicate();
}
