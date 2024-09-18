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
  ~Table();

  // Add, remove, and act on components.
  SparseId Add(MemberId owner);
  SparseId Duplicate(SparseId id, MemberId owner);
  void Remove(SparseId id);

  // Access component data and the owner of that component data.
  void* GetComponent(SparseId id) const;
  void* GetComponentAtDenseIndex(size_t denseIndex) const;
  MemberId GetOwnerAtDenseIndex(size_t denseIndex) const;
  SparseId GetSparseIdAtDenseIndex(size_t denseIndex) const;

  // Access private members that allow the component table to function.
  Comp::TypeId TypeId() const;
  const void* Data() const;
  size_t Stride() const;
  size_t Size() const;
  size_t Capacity() const;
  void VerifyId(SparseId id) const;
  void VerifyDenseIndex(size_t denseIndex) const;

  static constexpr size_t smStartCapacity = 10;
  static constexpr float smGrowthFactor = 2.0f;

private:
  Comp::TypeId mTypeId;
  Ds::SparseSet mSparseSet;
  char* mData;
  size_t mCapacity;
  Ds::Vector<MemberId> mOwners;

  SparseId AllocateComponent(MemberId owner);
  void Grow();
};

} // namespace World

#endif
