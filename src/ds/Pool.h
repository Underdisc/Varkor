#ifndef ds_Pool_h
#define ds_Pool_h

#include "ds/Vector.h"

namespace Ds {

typedef int PoolId;

template<typename T>
struct Pool
{
  constexpr static int smInvalidId = -1;
  PoolId Add(const T& element);
  void Remove(PoolId id);
  T& operator[](PoolId id);

  const Ds::Vector<T>& Data() const;
  const Ds::Vector<PoolId>& Dense() const;
  const Ds::Vector<int>& Sparse() const;

private:
  Ds::Vector<T> mData;
  Ds::Vector<PoolId> mDense;
  Ds::Vector<int> mSparse;
};

} // namespace Ds

#include "ds/Pool.hh"

#endif