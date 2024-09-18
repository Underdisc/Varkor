#include "ds/SparseSet.h"

namespace Ds {

SparseSet::SparseSet(): mDenseUsage(0) {}

SparseId SparseSet::Add()
{
  if (mDenseUsage < mDense.Size()) {
    mSparse[mDense[mDenseUsage]] = (int)mDenseUsage;
  }
  else {
    mSparse.Push((int)mSparse.Size());
    mDense.Push((SparseId)mDense.Size());
  }
  return mDense[mDenseUsage++];
}

void SparseSet::Request(SparseId id)
{
  // Ensure that the sparse array is large enough to include the requested id.
  if (id >= mSparse.Size()) {
    size_t oldSize = mSparse.Size();
    size_t newSize = id + 1;
    mSparse.Resize(newSize);
    for (size_t i = oldSize; i < newSize; ++i) {
      mSparse[i] = nInvalidSparseId;
    }
  }

  if (mSparse[id] != nInvalidSparseId) {
    LogAbort("The requested id is already being used.");
  }

  mSparse[id] = (int)mDenseUsage++;
  mDense.Push(id);
}

void SparseSet::Remove(SparseId id)
{
  mSparse[mDense[mDenseUsage - 1]] = mSparse[id];
  mDense.Swap(mSparse[id], --mDenseUsage);
  mSparse[id] = nInvalidSparseId;
}

void SparseSet::Clear()
{
  mDense.Clear();
  mSparse.Clear();
  mDenseUsage = 0;
}

bool SparseSet::Valid(SparseId id) const
{
  return id >= 0 && id < mSparse.Size() && mSparse[id] != nInvalidSparseId;
}

void SparseSet::Verify(SparseId id) const
{
  LogAbortIf(!Valid(id), "The provided PoolId is invalid.");
}

const Ds::Vector<SparseId>& SparseSet::Dense() const
{
  return mDense;
}

const Ds::Vector<int>& SparseSet::Sparse() const
{
  return mSparse;
}

size_t SparseSet::DenseUsage() const
{
  return mDenseUsage;
}

} // namespace Ds