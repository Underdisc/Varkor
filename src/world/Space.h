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

struct ComponentDescriptor
{
  void EndUse();
  bool InUse() const;
  Comp::TypeId mTypeId;
  SparseId mComponentId;
};

struct Member
{
public:
  Member();
  Member(DescriptorId firstDescriptorId);
  DescriptorId FirstDescriptorId() const;
  DescriptorId LastDescriptorId() const;
  DescriptorId DescriptorCount() const;

private:
  // The first DescriptorId and the number of ComponentDescriptors.
  DescriptorId mFirstDescriptorId;
  DescriptorId mDescriptorCount;
  DescriptorId EndDescriptorId() const;

  friend Space;
};

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
  Member& GetMember(MemberId id);
  const Member& GetConstMember(MemberId id) const;

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
  Ds::Vector<ComponentDescriptor> GetDescriptors(MemberId memberId) const;
  Ds::Vector<MemberId> RootMemberIds() const;

  // Private member access.
  const Ds::Map<Comp::TypeId, Table>& Tables() const;
  const Ds::Pool<Member>& Members() const;
  const Ds::Vector<MemberId>& UnusedMemberIds() const;
  const Ds::Vector<ComponentDescriptor>& DescriptorBin() const;

  void Serialize(Vlk::Value& spaceVal) const;
  Result Deserialize(const Vlk::Explorer& spaceEx);

private:
  Ds::Map<Comp::TypeId, Table> mTables;
  Ds::Pool<Member> mMembers;
  Ds::Vector<ComponentDescriptor> mDescriptorBin;

  void AddDescriptorToMember(
    MemberId memberId, const ComponentDescriptor& desc);
  bool ValidMemberId(MemberId memberId) const;
  void VerifyMemberId(MemberId memberId) const;

  friend World::Object;
};

} // namespace World

#include "Space.hh"

#endif
