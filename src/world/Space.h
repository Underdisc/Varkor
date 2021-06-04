#ifndef world_Space_h
#define world_Space_h

#include <string>

#include "comp/Type.h"
#include "ds/Map.h"
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
  Comp::TypeId mTypeId;
  int mTableIndex;
};

struct Member
{
public:
  std::string mName;

  int AddressIndex() const;
  int ComponentCount() const;
  MemberId Parent() const;
  const Ds::Vector<MemberId>& Children() const;
  bool HasParent() const;
  bool InUseRootMember() const;

private:
  // The index of the first component within the address bin.
  int mAddressIndex;
  static constexpr int smInvalidAddressIndex = -1;
  // The number of component addresses used by the member.
  int mComponentCount;
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

struct Space
{
  Space();
  Space(const std::string& name);
  void Update();

  // Member modification.
  MemberId CreateMember();
  MemberId CreateChildMember(MemberId parentId);
  void DeleteMember(MemberId member);
  void MakeParent(MemberId parent, MemberId child);
  void RemoveParent(MemberId childId);
  Member& GetMember(MemberId id);
  const Member& GetConstMember(MemberId id) const;

  // Component creation, deletion, and access.
  template<typename T>
  T& AddComponent(MemberId memberId);
  template<typename T>
  void RemComponent(MemberId memberId);
  template<typename T>
  T* GetComponent(MemberId memberId) const;
  template<typename T>
  bool HasComponent(MemberId memberId) const;

  void* AddComponent(Comp::TypeId typeId, MemberId memberId);
  void RemComponent(Comp::TypeId typeId, MemberId memberId);
  void* GetComponent(Comp::TypeId typeId, MemberId memberId) const;
  bool HasComponent(Comp::TypeId typeId, MemberId memberId) const;

  template<typename T, typename F>
  void VisitTable(F visit) const;
  template<typename F>
  void VisitActiveMemberIds(F visit) const;
  template<typename F>
  void VisitMemberComponentTypeIds(MemberId memberId, F visit) const;

  // Get the pointer to the beginning of a component table.
  template<typename T>
  const T* GetComponentData() const;
  const void* GetComponentData(Comp::TypeId typeId) const;

  // Private member access.
  const Ds::Map<Comp::TypeId, Table>& Tables() const;
  const Ds::Vector<Member>& Members() const;
  const Ds::Vector<MemberId>& UnusedMemberIds() const;
  const Ds::Vector<ComponentAddress> AddressBin() const;

public:
  std::string mName;
  MemberId mCameraId;

private:
  Ds::Map<Comp::TypeId, Table> mTables;
  Ds::Vector<Member> mMembers;
  Ds::Vector<MemberId> mUnusedMemberIds;
  Ds::Vector<ComponentAddress> mAddressBin;

  Table* GetTable(Comp::TypeId typeId) const;
  int GetTableIndex(Comp::TypeId typeId, MemberId memberId) const;
  bool ValidMemberId(MemberId memberId) const;
  void VerifyMemberId(MemberId memberId) const;

  friend World::Object;
};

} // namespace World

#include "Space.hh"

#endif
