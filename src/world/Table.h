#ifndef world_Table_h
#define world_Table_h

#include "ds/Vector.h"
#include "world/Types.h"

namespace World {

class Table
{
public:
  Table();
  Table(int stride);
  ~Table();

  // Add and remove components to and from the table.
  int Add(MemberId member);
  void Rem(int index);

  // Access component data and the owner of that component data.
  void* GetData(int index) const;
  MemberId GetOwner(int index) const;

  // Access private members that allow the component table to function.
  const void* Data() const;
  int Stride() const;
  int Size() const;
  int Capacity() const;

  static constexpr int smStartCapacity = 10;
  static constexpr float smGrowthFactor = 2.0f;
  static constexpr int smInvalidIndex = -1;

private:
  char* mData;
  int mStride;
  int mSize;
  int mCapacity;
  Ds::Vector<MemberId> mOwners;

  void Grow();
  void VerifyIndex(int index) const;
};

} // namespace World

#endif
