#include "debug/MemLeak.h"
#include "util/Memory.h"

#include "world/Table.h"

namespace World {

Table::Table(Comp::TypeId typeId):
  mData(nullptr), mTypeId(typeId), mSize(0), mCapacity(0), mOwners()
{}

Table::~Table()
{
  Comp::TypeData& typeData = Comp::nTypeData[mTypeId];
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
  Comp::TypeData& typeData = Comp::nTypeData[mTypeId];
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
  void* ogComponent = GetComponent(ogIndex);
  int newIndex = AllocateComponent(duplicateId);
  void* newComponent = GetComponent(newIndex);
  Comp::TypeData& typeData = Comp::nTypeData[mTypeId];
  typeData.mCopyConstruct(ogComponent, newComponent);
  return newIndex;
}

void Table::Rem(int index)
{
  VerifyIndex(index);
  mOwners[index] = nInvalidMemberId;
  void* component = GetComponent(index);
  Comp::TypeData& typeData = Comp::nTypeData[mTypeId];
  typeData.mDestruct(component);
}

void* Table::GetComponent(int index) const
{
  VerifyIndex(index);
  Comp::TypeData& typeData = Comp::nTypeData[mTypeId];
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
  Comp::TypeData& typeData = Comp::nTypeData[mTypeId];
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
  Comp::TypeData& typeData = Comp::nTypeData[mTypeId];
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
  Comp::TypeData& typeData = Comp::nTypeData[mTypeId];
  if (mData == nullptr)
  {
    mData = alloc char[smStartCapacity * typeData.mSize];
    mCapacity = smStartCapacity;
  } else
  {
    char* oldData = mData;
    mCapacity = (int)((float)mCapacity * smGrowthFactor);
    char* newData = alloc char[mCapacity * typeData.mSize];
    Util::Copy<char>(oldData, newData, mSize * typeData.mSize);
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
