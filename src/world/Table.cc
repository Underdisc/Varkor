#include "debug/MemLeak.h"
#include "util/Memory.h"

#include "world/Table.h"

namespace World {

Table::Table(Comp::TypeId typeId): mData(nullptr), mTypeId(typeId), mCapacity(0)
{}

Table::Table(Table&& other)
{
  mTypeId = other.mTypeId;
  mMemberIdToIndexMap = std::move(other.mMemberIdToIndexMap);
  mData = other.mData;
  mCapacity = other.mCapacity;

  other.mData = nullptr;
  mCapacity = 0;
}

Table::~Table()
{
  const Comp::TypeData& typeData = Comp::GetTypeData(mTypeId);
  for (int i = 0; i < mMemberIdToIndexMap.DenseUsage(); ++i) {
    typeData.mDestruct(GetComponentAtDenseIndex(i));
  }
  if (mData != nullptr) {
    delete[] mData;
  }
}

void* Table::Request(MemberId owner)
{
  void* component = AllocateComponent(owner);
  const Comp::TypeData& typeData = Comp::GetTypeData(mTypeId);
  typeData.mDefaultConstruct(component);
  return component;
}

void* Table::Duplicate(MemberId owner, MemberId duplicateOwner)
{
  void* duplicateComponent = AllocateComponent(duplicateOwner);
  void* component = GetComponent(owner);
  const Comp::TypeData& typeData = Comp::GetTypeData(mTypeId);
  typeData.mCopyConstruct(component, duplicateComponent);
  return duplicateComponent;
}

void Table::Remove(MemberId owner)
{
  VerifyComponent(owner);
  size_t removeIndex = mMemberIdToIndexMap.Sparse()[owner];
  size_t replaceIndex = mMemberIdToIndexMap.DenseUsage() - 1;
  const Comp::TypeData& typeData = Comp::GetTypeData(mTypeId);
  void* removedComponent = mData + typeData.mSize * removeIndex;
  void* replaceComponent = mData + typeData.mSize * replaceIndex;
  typeData.mMoveAssign(replaceComponent, removedComponent);
  typeData.mDestruct(replaceComponent);
  mMemberIdToIndexMap.Remove(owner);
}

void* Table::GetComponent(MemberId owner) const
{
  VerifyComponent(owner);
  size_t denseIndex = mMemberIdToIndexMap.Sparse()[owner];
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
  return mMemberIdToIndexMap.Dense()[denseIndex];
}

MemberId Table::GetMemberIdAtDenseIndex(size_t denseIndex) const
{
  VerifyDenseIndex(denseIndex);
  return mMemberIdToIndexMap.Dense()[denseIndex];
}

Comp::TypeId Table::TypeId() const
{
  return mTypeId;
}

const Ds::SparseSet& Table::MemberIdToIndexMap() const
{
  return mMemberIdToIndexMap;
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
  return mMemberIdToIndexMap.DenseUsage();
}

size_t Table::Capacity() const
{
  return mCapacity;
}

bool Table::ValidComponent(MemberId owner) const
{
  return mMemberIdToIndexMap.Valid(owner);
}

void Table::VerifyComponent(MemberId owner) const
{
  mMemberIdToIndexMap.Verify(owner);
}

void Table::VerifyDenseIndex(size_t denseIndex) const
{
  if (denseIndex >= mMemberIdToIndexMap.DenseUsage()) {
    LogAbort("The provided dense index is invalid.");
  }
}

void* Table::AllocateComponent(MemberId owner)
{
  if (mMemberIdToIndexMap.DenseUsage() == mCapacity) {
    Grow();
  }
  mMemberIdToIndexMap.Request(owner);
  return GetComponent(owner);
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
    for (int i = 0; i < mMemberIdToIndexMap.DenseUsage(); ++i) {
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
