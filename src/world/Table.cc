#include "debug/MemLeak.h"
#include "util/Memory.h"

#include "world/Table.h"

namespace World {

Table::Table(Comp::TypeId typeId):
  mData(nullptr), mTypeId(typeId), mSize(0), mCapacity(0), mOwners()
{}

Table::~Table()
{
  const Comp::TypeData& typeData = Comp::GetTypeData(mTypeId);
  for (int i = 0; i < mSize; ++i) {
    if (ActiveIndex(i)) {
      typeData.mDestruct(GetComponent(i));
    }
  }
  if (mData != nullptr) {
    delete[] mData;
  }
}

// This function will allow adding the same member id to the table multiple
// times. The Table is not responsible for managing the owner member ids it
// stores besides setting the member ids when components are added or removed.
size_t Table::Add(MemberId memberId)
{
  size_t index = AllocateComponent(memberId);
  void* component = GetComponent(index);
  const Comp::TypeData& typeData = Comp::GetTypeData(mTypeId);
  typeData.mDefaultConstruct(component);
  return index;
}

size_t Table::AllocateComponent(MemberId member)
{
  mOwners.Push(member);
  if (mSize >= mCapacity) {
    Grow();
  }
  return mSize++;
}

size_t Table::Duplicate(size_t ogIndex, MemberId duplicateId)
{
  VerifyIndex(ogIndex);
  size_t newIndex = AllocateComponent(duplicateId);
  void* ogComponent = GetComponent(ogIndex);
  void* newComponent = GetComponent(newIndex);
  const Comp::TypeData& typeData = Comp::GetTypeData(mTypeId);
  typeData.mCopyConstruct(ogComponent, newComponent);
  return newIndex;
}

void Table::Rem(size_t index)
{
  VerifyIndex(index);
  mOwners[index] = nInvalidMemberId;
  void* component = GetComponent(index);
  const Comp::TypeData& typeData = Comp::GetTypeData(mTypeId);
  typeData.mDestruct(component);
}

void* Table::operator[](size_t index) const
{
  return GetComponent(index);
}

void* Table::GetComponent(size_t index) const
{
  VerifyIndex(index);
  const Comp::TypeData& typeData = Comp::GetTypeData(mTypeId);
  return (void*)(mData + typeData.mSize * index);
}

MemberId Table::GetOwner(size_t index) const
{
  return mOwners[index];
}

bool Table::ActiveIndex(size_t index) const
{
  return mOwners[index] != nInvalidMemberId;
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
  return mSize;
}

size_t Table::Capacity() const
{
  return mCapacity;
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
    for (int i = 0; i < mSize; ++i) {
      if (!ActiveIndex(i)) {
        continue;
      }
      void* oldComponent = (void*)(oldData + i * typeData.mSize);
      void* newComponent = (void*)(newData + i * typeData.mSize);
      typeData.mMoveConstruct(oldComponent, newComponent);
      typeData.mDestruct(oldComponent);
    }
    mData = newData;
    delete[] oldData;
  }
}

void Table::VerifyIndex(size_t index) const
{
  LogAbortIf(
    index >= mSize || index < 0, "Provided Table index is out of range.");
}

} // namespace World
