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
  int Add(MemberId member);
  int AllocateComponent(MemberId member);
  int Duplicate(int index, MemberId duplicateId);
  void Rem(int index);
  void UpdateComponents() const;

  // Access component data and the owner of that component data.
  void* operator[](int index) const;
  void* GetComponent(int index) const;
  MemberId GetOwner(int index) const;

  // Access private members that allow the component table to function.
  const void* Data() const;
  int Stride() const;
  int Size() const;
  int Capacity() const;

  void VisitComponents(std::function<void(void*)> visit) const;
  void VisitActiveIndices(std::function<void(int)> visit) const;

  static constexpr int smStartCapacity = 10;
  static constexpr float smGrowthFactor = 2.0f;
  static constexpr int smInvalidIndex = -1;

private:
  char* mData;
  Comp::TypeId mTypeId;
  int mSize;
  int mCapacity;
  Ds::Vector<MemberId> mOwners;

  void Grow();
  void VerifyIndex(int index) const;
};

} // namespace World

#endif
