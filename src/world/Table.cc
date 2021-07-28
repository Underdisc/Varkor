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
  VisitComponents(
    [&typeData](void* component)
    {
      typeData.mDestruct(component);
    });
  if (mData != nullptr)
  {
    delete[] mData;
  }
}

// This function will allow adding the same member id to the table multiple
// times. The Table is not responsible for managing the owner member ids it
// stores besides setting the member ids when components are added or removed.
int Table::Add(MemberId memberId)
{
  int index = AllocateComponent(memberId);
  void* component = GetComponent(index);
  const Comp::TypeData& typeData = Comp::GetTypeData(mTypeId);
  typeData.mDefaultConstruct(component);
  return index;
}

int Table::AllocateComponent(MemberId member)
{
  mOwners.Push(member);
  if (mSize >= mCapacity)
  {
    Grow();
  }
  return mSize++;
}

int Table::Duplicate(int ogIndex, MemberId duplicateId)
{
  VerifyIndex(ogIndex);
  int newIndex = AllocateComponent(duplicateId);
  void* ogComponent = GetComponent(ogIndex);
  void* newComponent = GetComponent(newIndex);
  const Comp::TypeData& typeData = Comp::GetTypeData(mTypeId);
  typeData.mCopyConstruct(ogComponent, newComponent);
  return newIndex;
}

void Table::Rem(int index)
{
  VerifyIndex(index);
  mOwners[index] = nInvalidMemberId;
  void* component = GetComponent(index);
  const Comp::TypeData& typeData = Comp::GetTypeData(mTypeId);
  typeData.mDestruct(component);
}

void* Table::operator[](int index) const
{
  return GetComponent(index);
}

void* Table::GetComponent(int index) const
{
  VerifyIndex(index);
  const Comp::TypeData& typeData = Comp::GetTypeData(mTypeId);
  return (void*)(mData + typeData.mSize * index);
}

MemberId Table::GetOwner(int index) const
{
  return mOwners[index];
}

const void* Table::Data() const
{
  return (void*)mData;
}

int Table::Stride() const
{
  const Comp::TypeData& typeData = Comp::GetTypeData(mTypeId);
  return typeData.mSize;
}

int Table::Size() const
{
  return mSize;
}

int Table::Capacity() const
{
  return mCapacity;
}

void Table::UpdateComponents() const
{
  const Comp::TypeData& typeData = Comp::GetTypeData(mTypeId);
  if (!typeData.mVUpdate.Open())
  {
    return;
  }
  VisitComponents(
    [&typeData](void* component)
    {
      typeData.mVUpdate.Invoke(component);
    });
}

void Table::VisitComponents(std::function<void(void*)> visit) const
{
  VisitActiveIndices(
    [&](int index)
    {
      visit(GetComponent(index));
    });
}

void Table::VisitActiveIndices(std::function<void(int)> visit) const
{
  for (int i = 0; i < mSize; ++i)
  {
    if (GetOwner(i) != nInvalidMemberId)
    {
      visit(i);
    }
  }
}

void Table::Grow()
{
  const Comp::TypeData& typeData = Comp::GetTypeData(mTypeId);
  if (mData == nullptr)
  {
    mData = alloc char[smStartCapacity * typeData.mSize];
    mCapacity = smStartCapacity;
  } else
  {
    char* oldData = mData;
    mCapacity = (int)((float)mCapacity * smGrowthFactor);
    char* newData = alloc char[mCapacity * typeData.mSize];
    VisitActiveIndices(
      [&](int index)
      {
        void* oldComponent = (void*)(oldData + index * typeData.mSize);
        void* newComponent = (void*)(newData + index * typeData.mSize);
        typeData.mMoveConstruct(oldComponent, newComponent);
        typeData.mDestruct(oldComponent);
      });
    mData = newData;
    delete[] oldData;
  }
}

void Table::VerifyIndex(int index) const
{
  LogAbortIf(
    index >= mSize || index < 0, "Provided Table index is out of range.");
}

} // namespace World
