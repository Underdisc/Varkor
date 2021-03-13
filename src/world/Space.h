#ifndef world_Space_h
#define world_Space_h

#include <string>

#include "ds/Vector.h"
#include "world/Table.h"
#include "world/Types.h"

namespace World {

// Pre-declaration
struct Object;
struct Space;

struct ComponentAddress
{
  void EndUse();
  bool InUse() const;

  // The table the component is stored in.
  TableId mTable;
  // The offset of the component data within the table.
  int mIndex;
};

struct Member
{
public:
  std::string mName;

  int AddressIndex() const;
  ObjSizeT Count() const;
  const Ds::Vector<MemberId>& Children() const;
  bool HasParent() const;
  bool InUseRootMember() const;

private:
  // The index of the first component address within a space address bin.
  int mAddressIndex;
  static constexpr int smInvalidAddressIndex = -1;
  // The number of component addresses used by the member.
  ObjSizeT mCount;
  // These are values for tracking parent child relationships.
  MemberId mParent;
  Ds::Vector<MemberId> mChildren;

  void StartUse(int addressIndex, const std::string& name);
  void EndUse();
  bool InUse() const;
  int EndAddress() const;
  int LastAddress() const;

  friend Space;
};

// todo: Space's should keep a copy of their id.
struct Space
{
  Space();
  Space(const std::string& name);

  // Component Registration.
  TableId RegisterComponentType(int componentId, int size);
  template<typename T>
  void RegisterComponentType();

  // Member modification.
  MemberId CreateMember();
  void DeleteMember(MemberId member);
  void MakeParent(MemberId parent, MemberId child);
  void RemoveParent(MemberId childId);
  Member& GetMember(MemberId id);

  // Component creation, deletion, and access.
  void* AddComponent(int componentId, MemberId member);
  void RemComponent(int componentId, MemberId member);
  void* GetComponent(int componentId, MemberId member) const;
  bool HasComponent(int compnentId, MemberId member) const;
  template<typename T>
  T& AddComponent(MemberId member);
  template<typename T>
  void RemComponent(MemberId member);
  template<typename T>
  T* GetComponent(MemberId member) const;
  template<typename T>
  bool HasComponent(MemberId member) const;

  // Get the pointer to the beginning of a component table.
  const void* GetComponentData(int componentId) const;
  template<typename T>
  const T* GetComponentData() const;

  // Create a visitor for visiting all of the components in a table.
  template<typename T>
  Table::Visitor<T> CreateTableVisitor() const;

  // Create a visitor for visiting all of the members in a space.
  struct MemberVisitor
  {
    Member& CurrentMember() const;
    MemberId CurrentMemberId() const;
    void Next();
    bool End() const;

  private:
    Space& mSpace;
    MemberId mCurrentMember;
    MemberVisitor(Space& space);
    void ReachValidMember();

    friend Space;
  };
  MemberVisitor CreateMemberVisitor();

  // Private member access.
  const Ds::Vector<TableId>& TableLookup() const;
  const Ds::Vector<Table>& Tables() const;
  const Ds::Vector<Member>& Members() const;
  const Ds::Vector<MemberId>& UnusedMemberIds() const;
  const Ds::Vector<ComponentAddress> AddressBin() const;

public:
  std::string mName;

private:
  Ds::Vector<TableId> mTableLookup;
  Ds::Vector<Table> mTables;
  Ds::Vector<Member> mMembers;
  Ds::Vector<MemberId> mUnusedMemberIds;
  Ds::Vector<ComponentAddress> mAddressBin;

  bool ValidTable(int componentId) const;
  void VerifyTable(int componentId) const;
  void VerifyMember(MemberId member) const;

  friend World::Object;
};

} // namespace World

#include "Space.hh"

#endif
