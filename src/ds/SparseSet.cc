#include "ds/SparseSet.h"
#include "Error.h"
#include "debug/MemLeak.h"
#include "util/Memory.h"

namespace Ds {

const size_t SparseSet::smStartCapacity = 10;
const float SparseSet::smGrowthFactor = 2.0f;

SparseSet::SparseSet():
  mDense(nullptr), mSparse(nullptr), mCapacity(0), mDenseUsage(0)
{}

SparseSet::~SparseSet()
{
  Clear();
}

SparseSet::SparseSet(const SparseSet& other)
{
  *this = other;
}

SparseSet::SparseSet(SparseSet&& other)
{
  mDense = other.mDense;
  mSparse = other.mSparse;
  mCapacity = other.mCapacity;
  mDenseUsage = other.mDenseUsage;

  other.mDense = nullptr;
  other.mSparse = nullptr;
  other.mCapacity = 0;
  other.mDenseUsage = 0;
}

SparseSet& SparseSet::operator=(const SparseSet& other)
{
  size_t allocSize = other.mCapacity * (sizeof(SparseId) + sizeof(size_t));
  char* newData = alloc char[allocSize];
  mDense = (SparseId*)newData;
  mSparse = (size_t*)(newData + (other.mCapacity * sizeof(SparseId)));
  mCapacity = other.mCapacity;
  mDenseUsage = other.mDenseUsage;
  memcpy(mDense, other.mDense, allocSize);
  return *this;
}

SparseId SparseSet::Add()
{
  if (mDenseUsage >= mCapacity) {
    Grow();
  }
  return mDense[mDenseUsage++];
}

void SparseSet::Request(SparseId id)
{
  // Ensure that the sparse set is large enough to include the requested id.
  if (id >= mCapacity) {
    Grow((size_t)((float)(id + 1) * smGrowthFactor));
  }
  LogAbortIf(Valid(id), "The requested id is already being used.");

  size_t temp = mSparse[id];
  Util::Swap(mSparse, (size_t)id, (size_t)mDense[mDenseUsage]);
  Util::Swap(mDense, mDenseUsage, temp);
  ++mDenseUsage;
}

void SparseSet::Remove(SparseId id)
{
  Verify(id);
  size_t temp = mSparse[id];
  Util::Swap(mSparse, (size_t)id, (size_t)mDense[mDenseUsage - 1]);
  Util::Swap(mDense, mDenseUsage - 1, temp);
  --mDenseUsage;
}

void SparseSet::Clear()
{
  if (mDense != nullptr) {
    delete (char*)mDense;
    mDense = nullptr;
  }
  mSparse = nullptr;
  mCapacity = 0;
  mDenseUsage = 0;
}

bool SparseSet::Valid(SparseId id) const
{
  return id >= 0 && id < mCapacity && mSparse[id] < mDenseUsage;
}

void SparseSet::Verify(SparseId id) const
{
  LogAbortIf(!Valid(id), "The provided SparseId is invalid.");
}

const SparseId* SparseSet::Dense() const
{
  return mDense;
}

const size_t* SparseSet::Sparse() const
{
  return mSparse;
}

size_t SparseSet::Capacity() const
{
  return mCapacity;
}

size_t SparseSet::DenseUsage() const
{
  return mDenseUsage;
}

void SparseSet::Grow()
{
  if (mCapacity == 0) {
    Grow(smStartCapacity);
  }
  else {
    Grow((size_t)((float)mCapacity * smGrowthFactor));
  }
}

void SparseSet::Grow(size_t newCapacity)
{
  // Create a new allocation.
  char* oldData = (char*)mDense;
  size_t allocSize = newCapacity * (sizeof(SparseId) + sizeof(size_t));
  char* newData = alloc char[allocSize];
  if (oldData != nullptr) {
    // Copy the old dense span to the new memory.
    char* oldDense = oldData;
    char* newDense = newData;
    size_t oldDenseSize = mCapacity * sizeof(SparseId);
    memcpy(newDense, oldDense, oldDenseSize);

    // Copy the old sparse span to the new memory.
    size_t newDenseSize = newCapacity * sizeof(SparseId);
    char* oldSparse = oldDense + oldDenseSize;
    char* newSparse = newDense + newDenseSize;
    size_t oldSparseSize = mCapacity * sizeof(size_t);
    memcpy(newSparse, oldSparse, oldSparseSize);
    delete oldData;
  }
  mDense = (SparseId*)newData;
  mSparse = (size_t*)(newData + newCapacity * sizeof(SparseId));

  // Initialize all of the newly acquired memory.
  size_t oldCapacity = mCapacity;
  for (size_t i = oldCapacity; i < newCapacity; ++i) {
    mDense[i] = (SparseId)i;
    mSparse[i] = i;
  }
  mCapacity = newCapacity;
}

} // namespace Ds