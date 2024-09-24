#ifndef world_Space_h
#define world_Space_h

#include <functional>
#include <string>

#include "comp/Type.h"
#include "ds/Map.h"
#include "ds/Pool.h"
#include "ds/Vector.h"
#include "world/Table.h"
#include "world/Types.h"

namespace World {

struct Object;
struct Space;

struct Space
{
  Space();
  void Clear();
  void Update();

  // Member modification.
  MemberId CreateMember();
  MemberId CreateChildMember(MemberId parentId);
  Object CreateObject();
  Object CreateChildObject(const Object& parent);
  Object CreateChildObject(MemberId parentId);
  void DeleteMember(MemberId member, bool root = true);
  void TryDeleteMember(MemberId member);
  MemberId Duplicate(MemberId memberId, bool root = true);
  void MakeParent(MemberId parentId, MemberId childId);
  void TryRemoveParent(MemberId memberId);
  bool HasParent(MemberId memberId);
  bool HasChildren(MemberId memberId);

  // Component creation, deletion, and access.
  template<typename T>
  T& AddComponent(MemberId memberId);
  template<typename T>
  T& EnsureComponent(MemberId memberId);
  template<typename T>
  void RemComponent(MemberId memberId);
  template<typename T>
  T& GetComponent(MemberId memberId) const;
  template<typename T>
  T* TryGetComponent(MemberId memberId) const;
  template<typename T>
  bool HasComponent(MemberId memberId) const;

  template<typename T>
  T& Add(MemberId memberId);
  template<typename T>
  T& Ensure(MemberId memberId);
  template<typename T>
  void Rem(MemberId memberId);
  template<typename T>
  T& Get(MemberId memberId) const;
  template<typename T>
  T* TryGet(MemberId memberId) const;
  template<typename T>
  bool Has(MemberId memberId) const;

  void* AddComponent(Comp::TypeId typeId, MemberId memberId, bool init = true);
  void* EnsureComponent(Comp::TypeId typeId, MemberId memberId);
  void RemComponent(Comp::TypeId typeId, MemberId memberId);
  void* GetComponent(Comp::TypeId typeId, MemberId memberId) const;
  void* TryGetComponent(Comp::TypeId typeId, MemberId memberId) const;
  bool HasComponent(Comp::TypeId typeId, MemberId memberId) const;

  template<typename T>
  Ds::Vector<MemberId> Slice() const;
  Ds::Vector<MemberId> Slice(Comp::TypeId typeId) const;
  Ds::Vector<MemberId> RootMemberIds() const;
  Ds::Vector<Comp::TypeId> GetComponentTypes(MemberId owner) const;

  const Ds::Map<Comp::TypeId, Table>& Tables() const;

  void Serialize(Vlk::Value& spaceVal) const;
  Result Deserialize(const Vlk::Explorer& spaceEx);

private:
  Ds::SparseSet mMembers;
  Ds::Map<Comp::TypeId, Table> mTables;

  bool ValidMemberId(MemberId memberId) const;
  void VerifyMemberId(MemberId memberId) const;

  friend World::Object;
};

} // namespace World

#include "Space.hh"

#endif
