#ifndef core_Space_h
#define core_Space_h

#include "ComponentTable.h"
#include "Types.h"
#include "ds/Vector.h"

namespace Core {

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

  static int smInvalidAddressIndex;

  void EndUse();
  bool InUse() const;
  int EndAddress() const;
  int LastAddress() const;
};

struct Space
{
  TableRef RegisterComponentType(int componentId, int size);
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
  T& GetComponent(MemRef member);
  template<typename T>
  bool HasComponent(MemRef member);
  template<typename T>
  const T* GetComponentData() const;

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
  DS::Vector<TableRef> mTableLookup;
  DS::Vector<ComponentTable> mTables;

  DS::Vector<Member> mMembers;
  DS::Vector<MemRef> mUnusedMemRefs;
  DS::Vector<ComponentAddress> mAddressBin;

  bool ValidComponentTable(int componentId) const;
  void VerifyComponentTable(int componentId) const;
  void VerifyMember(MemRef member) const;
};

} // namespace Core

#include "Space.hh"

#endif
