#ifndef world_Space_h
#define world_Space_h

#include <functional>
#include <string>

#include "comp/Type.h"
#include "ds/Map.h"
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
  size_t mTableIndex;
};

struct Member
{
public:
  std::string mName;

  Member();
  DescriptorId FirstDescriptorId() const;
  DescriptorId LastDescriptorId() const;
  DescriptorId DescriptorCount() const;
  MemberId Parent() const;
  const Ds::Vector<MemberId>& Children() const;
  bool HasParent() const;
  bool InUse() const;
  bool InUseRootMember() const;

private:
  // The first DescriptorId and the number of ComponentDescriptors.
  DescriptorId mFirstDescriptorId;
  DescriptorId mDescriptorCount;
  // These are values for tracking parent child relationships.
  MemberId mParent;
  Ds::Vector<MemberId> mChildren;

  void StartUse(DescriptorId firstDescId, const std::string& name);
  void EndUse();
  DescriptorId EndDescriptorId() const;

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
  MemberId Duplicate(MemberId memberId, bool duplicationRoot = true);
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

  void* AddComponent(Comp::TypeId typeId, MemberId memberId, bool init = true);
  void RemComponent(Comp::TypeId typeId, MemberId memberId);
  void* GetComponent(Comp::TypeId typeId, MemberId memberId) const;
  bool HasComponent(Comp::TypeId typeId, MemberId memberId) const;

  template<typename T>
  void VisitTableComponents(
    std::function<void(World::MemberId, T&)> visit) const;
  void VisitRootMemberIds(std::function<void(World::MemberId)> visit) const;
  void VisitMemberComponents(
    MemberId memberId, std::function<void(Comp::TypeId, size_t)> visit) const;

  // Private member access.
  const Ds::Map<Comp::TypeId, Table>& Tables() const;
  const Ds::Vector<Member>& Members() const;
  const Ds::Vector<MemberId>& UnusedMemberIds() const;
  const Ds::Vector<ComponentDescriptor> DescriptorBin() const;

  void Serialize(Vlk::Value& spaceVal) const;
  void Deserialize(const Vlk::Explorer& spaceEx);

public:
  std::string mName;
  MemberId mCameraId;

private:
  Ds::Map<Comp::TypeId, Table> mTables;
  Ds::Vector<Member> mMembers;
  Ds::Vector<MemberId> mUnusedMemberIds;
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
