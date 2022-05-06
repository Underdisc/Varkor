#ifndef world_Table_h
#define world_Table_h

#include <functional>

#include "comp/Type.h"
#include "ds/Vector.h"
#include "world/Types.h"

namespace World {

class Table
{
public:
  Table(Comp::TypeId typeId);
  ~Table();

  // Add, remove, and act on components.
  size_t Add(MemberId member);
  size_t AllocateComponent(MemberId member);
  size_t Duplicate(size_t index, MemberId duplicateId);
  void Rem(size_t index);

  // Access component data and the owner of that component data.
  void* operator[](size_t index) const;
  void* GetComponent(size_t index) const;
  MemberId GetOwner(size_t index) const;
  bool ActiveIndex(size_t index) const;

  // Access private members that allow the component table to function.
  Comp::TypeId TypeId() const;
  const void* Data() const;
  size_t Stride() const;
  size_t Size() const;
  size_t Capacity() const;

  static constexpr size_t smStartCapacity = 10;
  static constexpr float smGrowthFactor = 2.0f;

private:
  char* mData;
  Comp::TypeId mTypeId;
  size_t mSize;
  size_t mCapacity;
  Ds::Vector<MemberId> mOwners;

  void Grow();
  void VerifyIndex(size_t index) const;
};

} // namespace World

#endif
