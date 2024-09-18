#ifndef ds_SparseSet_h
#define ds_SparseSet_h

#include "ds/Vector.h"

namespace Ds {

typedef int SparseId;
constexpr SparseId nInvalidSparseId = -1;

struct SparseSet
{
public:
  SparseSet();
  SparseId Add();
  void Request(SparseId id);
  void Remove(SparseId id);
  void Clear();
  bool Valid(SparseId id) const;
  void Verify(SparseId id) const;

  const Ds::Vector<SparseId>& Dense() const;
  const Ds::Vector<int>& Sparse() const;
  size_t DenseUsage() const;

protected:
  Ds::Vector<SparseId> mDense;
  Ds::Vector<int> mSparse;
  size_t mDenseUsage;
};

} // namespace Ds

typedef Ds::SparseId SparseId;

#endif