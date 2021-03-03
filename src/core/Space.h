#ifndef core_Space_h
#define core_Space_h

#include <string>

#include "Table.h"
#include "Types.h"
#include "ds/Vector.h"

namespace Core {

// Pre-declaration
namespace World {
struct Object;
}

struct ComponentAddress
{
  void EndUse();
  bool InUse() const;

  // The table the component is stored in.
  TableRef mTable;
  // The offset of the component data within the table.
  int mIndex;
};

struct Member
{
  void EndUse();
  bool InUse() const;
  int EndAddress() const;
  int LastAddress() const;

  static constexpr int smInvalidAddressIndex = -1;
  // The index of the first component address within a space address bin.
  int mAddressIndex;
  // The number of component addresses used by the member.
  ObjSizeT mCount;
  // todo: Remove names from the member structure. Names should only be
  // necessary for interacting with objects in the editor and saving them to
  // file. They should never used for any sort of gameplay code. A
  // SpaceInteraction struct could be built outside of the space to serve this
  // purpose. This may also get rid of the need for the MemberVisitor.
  std::string mName;
};

// todo: Space's should keep a copy of their id.
struct Space
{
  Space();
  Space(const std::string& name);

  // Component Registration.
  TableRef RegisterComponentType(int componentId, int size);
  template<typename T>
  void RegisterComponentType();

  // Member creation and deletion.
  MemRef CreateMember();
  void DeleteMember(MemRef member);

  // Component creation, deletion, and access.
  void* AddComponent(int componentId, MemRef member);
  void RemComponent(int componentId, MemRef member);
  void* GetComponent(int componentId, MemRef member) const;
  bool HasComponent(int compnentId, MemRef member) const;
  template<typename T>
  T& AddComponent(MemRef member);
  template<typename T>
  void RemComponent(MemRef member);
  template<typename T>
  T* GetComponent(MemRef member) const;
  template<typename T>
  bool HasComponent(MemRef member) const;

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
    Member& CurrentMember();
    MemRef CurrentMemberRef();
    void Next();
    bool End();

  private:
    Space& mSpace;
    MemRef mCurrentMember;
    MemberVisitor(Space& space);
    void ReachValidMember();

    friend Space;
  };
  MemberVisitor CreateMemberVisitor();

  // Private member access.
  const Ds::Vector<TableRef>& TableLookup() const;
  const Ds::Vector<Table>& Tables() const;
  const Ds::Vector<Member>& Members() const;
  const Ds::Vector<MemRef>& UnusedMemRefs() const;
  const Ds::Vector<ComponentAddress> AddressBin() const;

public:
  std::string mName;

private:
  Ds::Vector<TableRef> mTableLookup;
  Ds::Vector<Table> mTables;
  Ds::Vector<Member> mMembers;
  Ds::Vector<MemRef> mUnusedMemRefs;
  Ds::Vector<ComponentAddress> mAddressBin;

  bool ValidTable(int componentId) const;
  void VerifyTable(int componentId) const;
  void VerifyMember(MemRef member) const;

  friend World::Object;
};

} // namespace Core

#include "Space.hh"

#endif
