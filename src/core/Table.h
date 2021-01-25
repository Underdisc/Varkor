#ifndef core_Table_h
#define core_Table_h

#include "core/Types.h"
#include "ds/Vector.h"

namespace Core {

class Table
{
public:
  Table();
  Table(int stride);
  ~Table();
  int Add(MemRef member);
  void Rem(int index);
  void* operator[](int index) const;
  const void* Data() const;

  void ShowStats() const;
  void ShowOwners() const;

  const static int smStartCapacity;
  const static float smGrowthFactor;

private:
  // todo: Consider putting this into a RawVector. It's almost the same as a
  // typical generic vector. The exception is the stride value for tracking
  // element size rather than relying on type information.
  char* mData;
  int mStride;
  int mSize;
  int mCapacity;
  Ds::Vector<MemRef> mOwners;

  void Grow();
  void VerifyIndex(int index) const;
};

} // namespace Core

#endif
