#ifndef ds_Pool_h
#define ds_Pool_h

#include "ds/Vector.h"

namespace Ds {

typedef int PoolId;
constexpr PoolId nInvalidPoolId = -1;

template<typename T>
struct Pool
{
  PoolId Add(const T& element);
  T& Request(PoolId id);
  void Remove(PoolId id);
  void Clear();
  bool Valid(PoolId id) const;
  void Verify(PoolId id) const;
  T& GetWithDenseIndex(size_t denseIndex);
  const T& GetWithDenseIndex(size_t denseIndex) const;
  T& operator[](PoolId id);
  const T& operator[](PoolId id) const;

  size_t Size() const;
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