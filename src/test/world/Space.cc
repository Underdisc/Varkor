#include <iostream>

#include "comp/Relationship.h"
#include "comp/Type.h"
#include "debug/MemLeak.h"
#include "test/Test.h"
#include "test/world/Print.h"
#include "test/world/TestTypes.h"
#include "world/Space.h"
#include "world/Types.h"

void Registration()
{
  PrintRegistration<CallCounter>();
  PrintRegistration<Simple0>();
  PrintRegistration<Simple1>();
  PrintRegistration<Dynamic>();
  PrintRegistration<Container>();
  PrintRegistration<Dependant>();
  PrintRegistration<Comp::Relationship>();
}

void CreateMember()
{
  World::Space space;
  for (size_t i = 0; i < 20; ++i) {
    space.CreateMember();
  }
  PrintSpace(space);
}

void DeleteMember()
{
  World::Space space;
  World::MemberId memberIds[10];
  for (size_t i = 0; i < 10; ++i) {
    memberIds[i] = space.CreateMember();
  }

  for (size_t i = 0; i < 10; i += 2) {
    space.DeleteMember(memberIds[i]);
  }
  PrintSpace(space);
}

void RootMemberIds()
{
  World::Space space;
  MemberId mem0 = space.CreateMember();
  MemberId mem1 = space.CreateMember();
  space.DeleteMember(mem0);
  std::cout << space.RootMemberIds() << '\n';
}

void Relationships0()
{
  // Ensures that all children are deleted and not skipped over due to an
  // early removal of a child id from the relationship's children vector.
  World::Space space;
  MemberId parentId = space.CreateMember();
  for (int i = 0; i < 3; ++i) {
    space.CreateChildMember(parentId);
  }
  space.CreateMember();
  space.DeleteMember(parentId);
  PrintSpace(space);
}

void Relationships1()
{
  World::Space space;
  MemberId parentId = space.CreateMember();
  MemberId childId = space.CreateMember();
  MemberId grandchildId = space.CreateChildMember(childId);
  space.CreateChildMember(parentId);
  space.MakeParent(parentId, childId);
  space.DeleteMember(parentId);
  PrintSpace(space);
}

void Relationships2()
{
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

  PrintSpace(space);
  PrintTableOwners(
    space.Tables()[(SparseId)Comp::Type<Comp::Relationship>::smId]);
  PrintSpaceRelationships(space);

  // Delete the member with children and create members that take the MemberIds
  // that were once used by the children.
  space.DeleteMember(memberIds[0]);
  for (size_t i = 0; i < 11; ++i) {
    space.CreateMember();
  }

  // Delete a member that has a parent.
  space.DeleteMember(memberIds[9]);

  PrintSpace(space);
  PrintSpaceRelationships(space);
}

void AddComponent()
{
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

  PrintSpace(space);

  space.AddComponent<Container>(mem1).SetData(4);
  space.AddComponent<Container>(mem0).SetData(3);
  space.AddComponent<Dynamic>(mem2);

  PrintSpaceTablesStats(space);
  PrintSpaceTablesOwners(space);
  PrintSpace(space);
}

void RemComponent()
{
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

  space.RemComponent<Dynamic>(mem2);
  space.RemComponent<Simple0>(mem1);
  space.RemComponent<Simple1>(mem2);
  space.RemComponent<Simple1>(mem0);

  space.AddComponent<Simple0>(mem2);

  PrintSpace(space);
  PrintSpaceTablesStats(space);
  PrintSpaceTablesOwners(space);
}

void DeleteMembersWithComponents()
{
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
  PrintSpace(space);
  PrintSpaceTablesOwners(space);

  space.DeleteMember(memberIds[0]);
  space.DeleteMember(memberIds[3]);
  space.DeleteMember(memberIds[4]);
  space.DeleteMember(memberIds[6]);
  space.DeleteMember(memberIds[7]);
  PrintSpace(space);
  PrintSpaceTablesOwners(space);

  World::MemberId newMemberId = space.CreateMember();
  space.AddComponent<Simple0>(newMemberId);
  space.AddComponent<Simple1>(newMemberId);
  space.AddComponent<Dynamic>(newMemberId);
  newMemberId = space.CreateMember();
  space.AddComponent<Container>(newMemberId);
  space.AddComponent<Simple1>(newMemberId);
  PrintSpace(space);
  PrintSpaceTablesOwners(space);
}

void GetComponent()
{
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

  space.GetComponent<Simple0>(mem0).SetData(9);
  space.GetComponent<Simple1>(mem0).SetData(8);
  space.GetComponent<Dynamic>(mem0).SetData(7);
  space.GetComponent<Container>(mem0).SetData(6);
  space.GetComponent<Simple0>(mem1).SetData(9);
  space.GetComponent<Simple1>(mem1).SetData(8);
  space.GetComponent<Dynamic>(mem1).SetData(7);

  PrintSpaceTablesStats(space);
  PrintSpaceTablesOwners(space);
  PrintSpace(space);
}

void HasComponent()
{
  World::Space space;
  World::MemberId mem0 = space.CreateMember();
  space.AddComponent<Simple1>(mem0);
  space.AddComponent<Dynamic>(mem0);
  World::MemberId mem1 = space.CreateMember();
  space.AddComponent<Simple0>(mem1);
  space.AddComponent<Container>(mem1);
  space.AddComponent<Container>(mem0);

  PrintSpace(space);
  std::cout << space.HasComponent<Simple0>(mem0)
            << space.HasComponent<Simple1>(mem0)
            << space.HasComponent<Dynamic>(mem0)
            << space.HasComponent<Container>(mem0)
            << space.HasComponent<Simple0>(mem1)
            << space.HasComponent<Simple1>(mem1)
            << space.HasComponent<Dynamic>(mem1)
            << space.HasComponent<Container>(mem1) << '\n';
}

void Duplicate0()
{
  // Duplicate a lone member.
  World::Space space;
  World::MemberId testId = space.CreateMember();
  space.AddComponent<Simple0>(testId).SetData(0);
  space.AddComponent<Simple1>(testId).SetData(1);
  space.Duplicate(testId);
  PrintSpaceTablesOwners(space);
  PrintSpace(space);
}

void Duplicate1()
{
  // Duplicate a member with children.
  World::Space space;
  MemberId testId = space.CreateMember();
  space.AddComponent<Simple0>(testId).SetData(9);
  space.AddComponent<Simple1>(testId).SetData(8);
  World::MemberId childId = space.CreateChildMember(testId);
  space.AddComponent<Dynamic>(childId).SetData(7);
  space.AddComponent<Container>(childId).SetData(6);
  childId = space.CreateChildMember(testId);
  space.AddComponent<Dynamic>(childId).SetData(5);
  space.Duplicate(testId);
  PrintSpaceTablesOwners(space);
  PrintSpace(space);
  PrintSpaceRelationships(space);
}

void Duplicate2()
{
  // Duplicate a member with a parent and children.
  World::Space space;
  World::MemberId parentId = space.CreateMember();
  World::MemberId testId = space.CreateChildMember(parentId);
  space.AddComponent<Simple0>(testId).SetData(2);
  space.AddComponent<Dynamic>(testId).SetData(2);
  World::MemberId childId = space.CreateChildMember(testId);
  space.AddComponent<Simple0>(childId).SetData(2);
  space.AddComponent<Container>(childId).SetData(2);
  childId = space.CreateChildMember(testId);
  space.AddComponent<Simple1>(childId).SetData(2);
  space.AddComponent<Container>(childId).SetData(2);
  space.Duplicate(testId);
  PrintSpaceTablesOwners(space);
  PrintSpace(space);
  PrintSpaceRelationships(space);
}

void Duplicate3()
{
  // A combination of the last three duplication tests. This will force the
  // table for the relationship components to grow. The test makes sure that an
  // access attempt on an invalidated relationship component isn't made.
  World::Space space;
  World::MemberId testId = space.CreateMember();
  space.AddComponent<Simple0>(testId).SetData(0);
  space.AddComponent<Simple1>(testId).SetData(0);
  space.Duplicate(testId);

  testId = space.CreateMember();
  space.AddComponent<Simple0>(testId).SetData(1);
  space.AddComponent<Simple1>(testId).SetData(1);
  World::MemberId childId = space.CreateChildMember(testId);
  space.AddComponent<Dynamic>(childId).SetData(1);
  space.AddComponent<Container>(childId).SetData(1);
  childId = space.CreateChildMember(testId);
  space.AddComponent<Dynamic>(childId).SetData(1);
  space.Duplicate(testId);

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
  PrintSpaceTablesOwners(space);
  PrintSpace(space);
  PrintSpaceRelationships(space);
}

void Dependencies()
{
  World::Space space;

  World::MemberId mem0 = space.CreateMember();
  space.AddComponent<CallCounter>(mem0);
  space.AddComponent<Dynamic>(mem0);
  space.AddComponent<Dependant>(mem0);
  World::MemberId mem1 = space.CreateMember();
  space.AddComponent<Dependant>(mem1);
  World::MemberId mem2 = space.CreateMember();
  space.AddComponent<Dependant>(mem2);

  space.RemComponent<CallCounter>(mem0);
  space.RemComponent<Dynamic>(mem1);
  space.RemComponent<Dependant>(mem2);
  PrintSpace(space);
}

void Slice()
{
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

int main(void)
{
  Error::Init();
  RegisterComponentTypes();

  RunTest(Registration);
  RunTest(CreateMember);
  RunTest(DeleteMember);
  RunTest(RootMemberIds);
  RunTest(Relationships0);
  RunTest(Relationships1);
  RunTest(Relationships2);
  RunTest(AddComponent);
  RunTest(RemComponent);
  RunTest(DeleteMembersWithComponents);
  RunTest(GetComponent);
  RunTest(HasComponent);
  RunTest(Duplicate0);
  RunTest(Duplicate1);
  RunTest(Duplicate2);
  RunTest(Duplicate3);
  RunCallCounterTest(Dependencies);
  RunTest(Slice);
}
