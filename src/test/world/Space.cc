#include <iostream>

#include "comp/Type.h"
#include "debug/MemLeak.h"
#include "test/world/Print.h"
#include "test/world/TestTypes.h"
#include "world/Space.h"
#include "world/Types.h"

void Registration()
{
  std::cout << "<= Registration =>\n";
  PrintRegistration<CallCounter>();
  PrintRegistration<Simple0>();
  PrintRegistration<Simple1>();
  PrintRegistration<Dynamic>();
  PrintRegistration<Container>();
  PrintRegistration<Dependant>();
}

void CreateMember()
{
  std::cout << "<= CreateMember =>" << std::endl;
  World::Space space;
  for (size_t i = 0; i < 20; ++i) {
    space.CreateMember();
  }
  PrintSpaceMembers(space);
}

void DeleteMember()
{
  std::cout << "<= DeleteMember =>" << std::endl;
  World::Space space;
  World::MemberId memberIds[10];
  for (size_t i = 0; i < 10; ++i) {
    memberIds[i] = space.CreateMember();
  }

  for (size_t i = 0; i < 10; i += 2) {
    space.DeleteMember(memberIds[i]);
  }
  PrintSpaceMembers(space);
  PrintSpaceUnusedMemberIds(space);
}

void ParentChildMembers()
{
  std::cout << "<= ParentChildMembers =>" << std::endl;
  World::Space space;
  World::MemberId memberIds[10];
  for (size_t i = 0; i < 10; ++i) {
    memberIds[i] = space.CreateMember();
  }
  space.MakeParent(memberIds[0], memberIds[1]);
  space.MakeParent(memberIds[0], memberIds[2]);
  space.MakeParent(memberIds[0], memberIds[3]);
  space.CreateChildMember(memberIds[0]);

  space.MakeParent(memberIds[1], memberIds[4]);
  space.CreateChildMember(memberIds[1]);

  space.MakeParent(memberIds[2], memberIds[5]);
  space.CreateChildMember(memberIds[2]);

  space.MakeParent(memberIds[3], memberIds[6]);
  space.MakeParent(memberIds[3], memberIds[7]);
  space.CreateChildMember(memberIds[3]);

  space.MakeParent(memberIds[8], memberIds[9]);
  space.CreateChildMember(memberIds[8]);

  PrintSpaceMembers(space);
  PrintSpaceRelationships(space);

  // Delete the member with children and create members that take the MemberIds
  // that were once used by the children.
  space.DeleteMember(memberIds[0]);
  for (size_t i = 0; i < 11; ++i) {
    space.CreateMember();
  }

  // Delete a member that has a parent.
  space.DeleteMember(memberIds[9]);

  PrintSpaceMembers(space);
  PrintSpaceRelationships(space);
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
  space.AddComponent<Container>(mem2).SetData(5);

  PrintSpaceMembers(space);
  PrintSpaceDescriptorBin(space);

  space.AddComponent<Container>(mem1).SetData(4);
  space.AddComponent<Container>(mem0).SetData(3);
  space.AddComponent<Dynamic>(mem2);

  PrintSpace(space);
  PrintSpaceTestTypeComponentData(space);
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
  PrintSpaceTablesStats(space);
  PrintSpaceTablesOwners(space);
}

void DeleteMembersWithComponents()
{
  std::cout << "<= DeleteMembersWithComponents =>" << std::endl;
  World::Space space;
  World::MemberId memberIds[8];
  for (size_t i = 0; i < 8; ++i) {
    memberIds[i] = space.CreateMember();
    space.AddComponent<Simple0>(memberIds[i]);
    if (i % 2 == 0) {
      space.AddComponent<Simple1>(memberIds[i]);
    }
    if (i % 3 == 0) {
      space.AddComponent<Dynamic>(memberIds[i]);
    }
    if (i % 5 == 0) {
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

  Simple0& mem0comp0 = space.GetComponent<Simple0>(mem0);
  mem0comp0.SetData(0);
  Simple1& mem0comp1 = space.GetComponent<Simple1>(mem0);
  mem0comp1.SetData(1);
  Dynamic& mem0comp2 = space.GetComponent<Dynamic>(mem0);
  mem0comp2.SetData(2);
  Container& mem0comp3 = space.GetComponent<Container>(mem0);
  mem0comp3.SetData(3);
  Simple0& mem1comp0 = space.GetComponent<Simple0>(mem1);
  mem1comp0.SetData(1);
  Simple1& mem1comp1 = space.GetComponent<Simple1>(mem1);
  mem1comp1.SetData(2);
  Dynamic& mem1comp2 = space.GetComponent<Dynamic>(mem1);
  mem1comp2.SetData(3);

  PrintSpace(space);
  PrintSpaceTestTypeComponentData(space);
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
            << space.HasComponent<Simple0>(World::nInvalidMemberId) << '\n';
}

void Duplicate()
{
  std::cout << "<= Duplicate =>" << std::endl;
  World::Space space;

  // A lone member.
  World::MemberId testId = space.CreateMember();
  space.AddComponent<Simple0>(testId).SetData(0);
  space.AddComponent<Simple1>(testId).SetData(0);
  space.Duplicate(testId);

  // A member with children.
  testId = space.CreateMember();
  space.AddComponent<Simple0>(testId).SetData(1);
  space.AddComponent<Simple1>(testId).SetData(1);
  World::MemberId childId = space.CreateChildMember(testId);
  space.AddComponent<Dynamic>(childId).SetData(1);
  space.AddComponent<Container>(childId).SetData(1);
  childId = space.CreateChildMember(testId);
  space.AddComponent<Dynamic>(childId).SetData(1);
  space.Duplicate(testId);

  // A member with a parent and children.
  World::MemberId parentId = space.CreateMember();
  testId = space.CreateChildMember(parentId);
  space.AddComponent<Simple0>(testId).SetData(2);
  space.AddComponent<Dynamic>(testId).SetData(2);
  childId = space.CreateChildMember(testId);
  space.AddComponent<Simple0>(childId).SetData(2);
  space.AddComponent<Container>(childId).SetData(2);
  childId = space.CreateChildMember(testId);
  space.AddComponent<Simple1>(childId).SetData(2);
  space.AddComponent<Container>(childId).SetData(2);
  space.Duplicate(testId);

  PrintSpaceMembers(space);
  PrintSpaceDescriptorBin(space);
  PrintSpaceRelationships(space);
  PrintSpaceTestTypeComponentData(space);
}

void Dependencies()
{
  std::cout << "<= Dependencies =>\n";
  World::Space space;

  World::MemberId mem0 = space.CreateMember();
  space.AddComponent<CallCounter>(mem0);
  space.AddComponent<Dynamic>(mem0);
  space.AddComponent<Dependant>(mem0);
  World::MemberId mem1 = space.CreateMember();
  space.AddComponent<Dependant>(mem1);
  World::MemberId mem2 = space.CreateMember();
  space.AddComponent<Dependant>(mem2);
  std::cout << "=Add=\n";
  PrintSpaceMembers(space);
  PrintSpaceDescriptorBin(space);

  space.RemComponent<CallCounter>(mem0);
  space.RemComponent<Dynamic>(mem1);
  space.RemComponent<Dependant>(mem2);
  std::cout << "=Rem=\n";
  PrintSpaceMembers(space);
  PrintSpaceDescriptorBin(space);
  CallCounter::Print();
  CallCounter::Reset();
}

void Slice()
{
  std::cout << "<= Slice =>\n";
  World::Space space;
  for (int i = 0; i < 20; ++i) {
    World::MemberId memberId = space.CreateMember();
    if (i < 10) {
      space.AddComponent<Simple0>(memberId);
    }
    if (i % 2 == 0) {
      space.AddComponent<Simple1>(memberId);
    }
    if (i >= 10) {
      space.AddComponent<Dynamic>(memberId);
    }
  }

  auto printMemberVector = [](const Ds::Vector<World::MemberId>& ids)
  {
    for (int i = 0; i < ids.Size(); ++i) {
      std::cout << ' ' << ids[i];
    }
    std::cout << '\n';
  };
  Ds::Vector<World::MemberId> simple0Slice = space.Slice<Simple0>();
  std::cout << "Simple0:";
  printMemberVector(simple0Slice);
  Ds::Vector<World::MemberId> simple1Slice = space.Slice<Simple1>();
  std::cout << "Simple1:";
  printMemberVector(simple1Slice);
  Ds::Vector<World::MemberId> dynamicSlice = space.Slice<Dynamic>();
  std::cout << "Dynamic:";
  printMemberVector(dynamicSlice);
}

void RunTest(void (*test)())
{
  static bool firstTest = true;
  if (!firstTest) {
    std::cout << '\n';
    CallCounter::Reset();
  }
  test();
  firstTest = false;
}

int main(void)
{
  EnableLeakOutput();
  RegisterComponentTypes();
  PrintKey();

  RunTest(Registration);
  RunTest(CreateMember);
  RunTest(DeleteMember);
  RunTest(ParentChildMembers);
  RunTest(AddComponent);
  RunTest(RemComponent);
  RunTest(DeleteMembersWithComponents);
  RunTest(GetComponent);
  RunTest(HasComponent);
  RunTest(Duplicate);
  RunTest(Dependencies);
  RunTest(Slice);
}
