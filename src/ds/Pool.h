#ifndef ds_Pool_h
#define ds_Pool_h

#include "ds/SparseSet.h"

namespace Ds {

typedef SparseId PoolId;
constexpr PoolId nInvalidPoolId = nInvalidSparseId;

template<typename T>
struct Pool: public SparseSet
{
  PoolId Add(const T& element);
  T& Request(PoolId id);
  void Remove(PoolId id);
  void Clear();
  T& GetWithDenseIndex(size_t denseIndex);
  const T& GetWithDenseIndex(size_t denseIndex) const;
  T& operator[](PoolId id);
  const T& operator[](PoolId id) const;

  const Ds::Vector<T>& Data() const;

private:
  Ds::Vector<T> mData;
};

} // namespace Ds

#include "ds/Pool.hh"

#endif