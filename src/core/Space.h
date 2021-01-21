#ifndef core_Space_h
#define core_Space_h

#include <string>

#include "ComponentTable.h"
#include "Types.h"
#include "ds/Vector.h"

namespace Core {

namespace World {
struct Object;
}

struct ComponentAddress
{
  TableRef mTable;
  int mIndex;

  void EndUse();
  bool InUse() const;
};

struct Member
{
  int mAddressIndex;
  ObjSizeT mCount;
  // todo: Remove names from the member structure. Names should only be
  // necessary for interacting with objects in the editor and saving them to
  // file. They should never used for any sort of gameplay code. A
  // SpaceInteraction struct could be built outside of the space to serve this
  // purpose. This may also get rid of the need for the MemberVisitor.
  std::string mName;

  void EndUse();
  bool InUse() const;
  int EndAddress() const;
  int LastAddress() const;
  static constexpr int smInvalidAddressIndex = -1;
};

struct Space
{
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

  std::string mName;

  Space();
  Space(const std::string& name);
  TableRef RegisterComponentType(int componentId, int size);
  MemberVisitor CreateMemberVisitor();
  MemRef CreateMember();
  void DeleteMember(MemRef member);
  void* AddComponent(int componentId, MemRef member);
  void RemComponent(int componentId, MemRef member);
  void* GetComponent(int componentId, MemRef member) const;
  bool HasComponent(int compnentId, MemRef member) const;
  const void* GetComponentData(int componentId) const;

  // Any component type that is registered within a Space should be tightly
  // packed. Not doing so won't necessarily result in errors, but problems will
  // likely appear if the padding of a component's data varies among different
  // compilers.
  template<typename T>
  void RegisterComponentType();

  // Component types that have not been registered with a Space will be if they
  // are added to a member as a component.
  template<typename T>
  T& AddComponent(MemRef member);
  template<typename T>
  void RemComponent(MemRef member);
  template<typename T>
  T* GetComponent(MemRef member);
  template<typename T>
  bool HasComponent(MemRef member);
  template<typename T>
  const T* GetComponentData() const;

  // todo: Remove all of the functions used for printing out data about a space.
  // This is only used by unit tests and the unit tests should take the
  // responsibility of printing this information out.
  void ShowAll() const;
  void ShowTableLookup() const;
  void ShowTable(int componentId) const;
  void ShowTables() const;
  void ShowOwnersInTables() const;
  void ShowMembers() const;
  void ShowAddressBin() const;
  void ShowUnusedMemRefs() const;

  template<typename T>
  void ShowTable() const;

private:
  Ds::Vector<TableRef> mTableLookup;
  Ds::Vector<ComponentTable> mTables;

  Ds::Vector<Member> mMembers;
  Ds::Vector<MemRef> mUnusedMemRefs;
  Ds::Vector<ComponentAddress> mAddressBin;

  bool ValidComponentTable(int componentId) const;
  void VerifyComponentTable(int componentId) const;
  void VerifyMember(MemRef member) const;

  friend World::Object;
};

} // namespace Core

#include "Space.hh"

#endif
