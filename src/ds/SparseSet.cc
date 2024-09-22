#include "ds/SparseSet.h"

namespace Ds {

SparseSet::SparseSet(): mDenseUsage(0) {}

SparseId SparseSet::Add()
{
  if (mDenseUsage == mDense.Size()) {
    mSparse.Push((int)mSparse.Size());
    mDense.Push((SparseId)mDense.Size());
  }
  return mDense[mDenseUsage++];
}

void SparseSet::Request(SparseId id)
{
  // Ensure that the sparse set is large enough to include the requested id.
  if (id >= mSparse.Size()) {
    size_t oldSize = mSparse.Size();
    size_t newSize = id + 1;
    mSparse.Resize(newSize);
    mDense.Resize(newSize);
    for (size_t i = oldSize; i < newSize; ++i) {
      mSparse[i] = i;
      mDense[i] = (SparseId)i;
    }
  }
  LogAbortIf(Valid(id), "The requested id is already being used.");

  mSparse.Swap(id, mDense[mDenseUsage]);
  mDense.Swap(mDenseUsage, mSparse[mDense[mDenseUsage]]);
  ++mDenseUsage;
}

void SparseSet::Remove(SparseId id)
{
  Verify(id);
  mDense.Swap(mDenseUsage - 1, mSparse[id]);
  mSparse.Swap(id, mDense[mSparse[id]]);
  --mDenseUsage;
}

void SparseSet::Clear()
{
  mDense.Clear();
  mSparse.Clear();
  mDenseUsage = 0;
}

bool SparseSet::Valid(SparseId id) const
{
  return id >= 0 && id < mSparse.Size() && mSparse[id] < mDenseUsage;
}

void SparseSet::Verify(SparseId id) const
{
  LogAbortIf(!Valid(id), "The provided SparseId is invalid.");
}

const Ds::Vector<SparseId>& SparseSet::Dense() const
{
  return mDense;
}

const Ds::Vector<size_t>& SparseSet::Sparse() const
{
  return mSparse;
}

size_t SparseSet::DenseUsage() const
{
  return mDenseUsage;
}

} // namespace Ds