#ifndef core_ComponentTable_h
#define core_ComponentTable_h

#include "ds/Vector.hh"
#include "CoreTypes.h"

// There are no constructors for the ComponentTable because no default
// constructor would make sense. Every ComponentTable must be initialized with
// a stride using Init().
class ComponentTable
{
public:
  void Init(int stride);
  ~ComponentTable();
  int Create(ObjRef object);
  void* operator[](int index);
  void ShowStats();

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

  DS::Vector<ObjRef> mOwners;
  DS::Vector<bool> mInUse;

  void Grow();
};

#endif
