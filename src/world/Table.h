#ifndef world_Table_h
#define world_Table_h

#include <functional>

#include "comp/Type.h"
#include "ds/Pool.h"
#include "world/Types.h"

namespace World {

class Table
{
public:
  Table(Comp::TypeId typeId);
  Table(Table&& other);
  ~Table();

  // Add, remove, and act on components.
  void* Request(MemberId owner);
  void* Duplicate(MemberId owner, MemberId duplicateOwner);
  void Remove(MemberId owner);

  // Access component data and the owner of that component data.
  void* GetComponent(MemberId owner) const;
  void* GetComponentAtDenseIndex(size_t denseIndex) const;
  MemberId GetOwnerAtDenseIndex(size_t denseIndex) const;
  MemberId GetMemberIdAtDenseIndex(size_t denseIndex) const;

  // Access private members that allow the component table to function.
  Comp::TypeId TypeId() const;
  const Ds::SparseSet& MemberIdToIndexMap() const;
  const void* Data() const;
  size_t Stride() const;
  size_t Size() const;
  size_t Capacity() const;
  bool ValidComponent(MemberId owner) const;
  void VerifyComponent(MemberId owner) const;
  void VerifyDenseIndex(size_t denseIndex) const;

  static constexpr size_t smStartCapacity = 10;
  static constexpr float smGrowthFactor = 2.0f;

private:
  Comp::TypeId mTypeId;
  Ds::SparseSet mMemberIdToIndexMap;
  char* mData;
  size_t mCapacity;

  void* AllocateComponent(MemberId owner);
  void Grow();
};

} // namespace World

#endif
