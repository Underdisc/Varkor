#include "debug/MemLeak.h"
#include "util/Memory.h"

#include "world/Table.h"

namespace World {

Table::Table(Comp::TypeId typeId): mData(nullptr), mTypeId(typeId), mCapacity(0)
{}

Table::~Table()
{
  const Comp::TypeData& typeData = Comp::GetTypeData(mTypeId);
  for (int i = 0; i < mSparseSet.DenseUsage(); ++i) {
    typeData.mDestruct(GetComponentAtDenseIndex(i));
  }
  if (mData != nullptr) {
    delete[] mData;
  }
}
// This function allows adding the same member id to the table multiple times.
SparseId Table::Add(MemberId owner)
{
  SparseId id = AllocateComponent(owner);
  void* component = GetComponent(id);
  const Comp::TypeData& typeData = Comp::GetTypeData(mTypeId);
  typeData.mDefaultConstruct(component);
  return id;
}

SparseId Table::Duplicate(SparseId id, MemberId owner)
{
  SparseId newId = AllocateComponent(owner);
  void* component = GetComponent(id);
  void* newComponent = GetComponent(newId);
  const Comp::TypeData& typeData = Comp::GetTypeData(mTypeId);
  typeData.mCopyConstruct(component, newComponent);
  return newId;
}

void Table::Remove(SparseId id)
{
  mSparseSet.Verify(id);
  int removeIndex = mSparseSet.Sparse()[id];
  size_t replaceIndex = mSparseSet.DenseUsage() - 1;
  const Comp::TypeData& typeData = Comp::GetTypeData(mTypeId);
  void* removedComponent = mData + typeData.mSize * removeIndex;
  void* replaceComponent = mData + typeData.mSize * replaceIndex;
  typeData.mMoveAssign(replaceComponent, removedComponent);
  typeData.mDestruct(replaceComponent);
  mOwners.LazyRemove(removeIndex);
  mSparseSet.Remove(id);
}

void* Table::GetComponent(SparseId id) const
{
  VerifyId(id);
  int denseIndex = mSparseSet.Sparse()[id];
  return GetComponentAtDenseIndex(denseIndex);
}

void* Table::GetComponentAtDenseIndex(size_t denseIndex) const
{
  VerifyDenseIndex(denseIndex);
  const Comp::TypeData& typeData = Comp::GetTypeData(mTypeId);
  return (void*)(mData + typeData.mSize * denseIndex);
}

MemberId Table::GetOwnerAtDenseIndex(size_t denseIndex) const
{
  VerifyDenseIndex(denseIndex);
  return mOwners[denseIndex];
}

SparseId Table::GetSparseIdAtDenseIndex(size_t denseIndex) const
{
  VerifyDenseIndex(denseIndex);
  return mSparseSet.Dense()[denseIndex];
}

Comp::TypeId Table::TypeId() const
{
  return mTypeId;
}

const void* Table::Data() const
{
  return (void*)mData;
}

size_t Table::Stride() const
{
  const Comp::TypeData& typeData = Comp::GetTypeData(mTypeId);
  return typeData.mSize;
}

size_t Table::Size() const
{
  return mSparseSet.DenseUsage();
}

size_t Table::Capacity() const
{
  return mCapacity;
}

void Table::VerifyId(SparseId id) const
{
  mSparseSet.Verify(id);
}

void Table::VerifyDenseIndex(size_t denseIndex) const
{
  if (denseIndex >= mSparseSet.DenseUsage()) {
    LogAbort("The provided dense index is invalid.");
  }
}

SparseId Table::AllocateComponent(MemberId owner)
{
  if (mSparseSet.DenseUsage() == mCapacity) {
    Grow();
  }
  mOwners.Push(owner);
  return mSparseSet.Add();
}

void Table::Grow()
{
  const Comp::TypeData& typeData = Comp::GetTypeData(mTypeId);
  if (mData == nullptr) {
    mData = alloc char[smStartCapacity * typeData.mSize];
    mCapacity = smStartCapacity;
  }
  else {
    char* oldData = mData;
    mCapacity = (size_t)((float)mCapacity * smGrowthFactor);
    char* newData = alloc char[mCapacity * typeData.mSize];
    for (int i = 0; i < mSparseSet.DenseUsage(); ++i) {
      void* oldComponent = (void*)(oldData + i * typeData.mSize);
      void* newComponent = (void*)(newData + i * typeData.mSize);
      typeData.mMoveConstruct(oldComponent, newComponent);
      typeData.mDestruct(oldComponent);
    }
    mData = newData;
    delete[] oldData;
  }
}

} // namespace World
