#ifndef ds_SparseSet_h
#define ds_SparseSet_h

#include <cstddef>

namespace Ds {

typedef int SparseId;
constexpr SparseId nInvalidSparseId = -1;

struct SparseSet {
public:
  SparseSet();
  ~SparseSet();
  SparseSet(const SparseSet& other);
  SparseSet(SparseSet&& other);
  SparseSet& operator=(const SparseSet& other);
  SparseSet& operator=(SparseSet&& other);

  SparseId Add();
  void Request(SparseId id);
  void Remove(SparseId id);
  void Clear();
  bool Valid(SparseId id) const;
  void Verify(SparseId id) const;

  const SparseId* Dense() const;
  const size_t* Sparse() const;
  size_t Capacity() const;
  size_t DenseUsage() const;

  static const size_t smStartCapacity;
  static const float smGrowthFactor;

protected:
  void Grow();
  void Grow(size_t newCapacity);

  SparseId* mDense;
  size_t* mSparse;
  size_t mCapacity;
  size_t mDenseUsage;
};

} // namespace Ds

typedef Ds::SparseId SparseId;

#endif
