#include <iostream>

#include "debug/MemLeak.h"
#include "util/Memory.h"

#include "Table.h"

namespace Core {

const int Table::smStartCapacity = 10;
const float Table::smGrowthFactor = 2.0f;

Table::Table(): mData(nullptr), mStride(0), mSize(0), mCapacity(0), mOwners() {}

Table::Table(int stride):
  mData(nullptr), mStride(stride), mSize(0), mCapacity(0), mOwners()
{}

Table::~Table()
{
  if (mData != nullptr)
  {
    delete[] mData;
  }
}

// This function will allow adding the same member reference to the table
// multiple times. The Table is not responsible for managing the owner
// member references it stores besides setting the member references when
// components are added or removed.
int Table::Add(MemRef member)
{
  if (mSize >= mCapacity)
  {
    Grow();
  }
  int componentIndex = mSize;
  ++mSize;
  mOwners.Push(member);
  return componentIndex;
}

void Table::Rem(int index)
{
  VerifyIndex(index);
  mOwners[index] = nInvalidMemRef;
}

void* Table::operator[](int index) const
{
  VerifyIndex(index);
  return (void*)(mData + (mStride * index));
}

const void* Table::Data() const
{
  return (void*)mData;
}

int Table::Size() const
{
  return mSize;
}

MemRef Table::GetOwner(int index) const
{
  return mOwners[index];
}

void Table::ShowStats() const
{
  std::cout << "Stride: " << mStride << std::endl
            << "Size: " << mSize << std::endl
            << "SizeInBytes: " << mSize * mStride << std::endl
            << "Capacity: " << mCapacity << std::endl
            << "CapacityInBytes: " << mCapacity * mStride << std::endl;
}

void Table::ShowOwners() const
{
  std::cout << "Owners: ";
  if (mSize == 0)
  {
    std::cout << "[]" << std::endl;
    return;
  }
  std::cout << "[" << mOwners[0];
  for (int i = 1; i < mSize; ++i)
  {
    std::cout << ", " << mOwners[i];
  }
  std::cout << "]" << std::endl;
}

void Table::Grow()
{
  if (mData == nullptr)
  {
    mData = alloc char[smStartCapacity * mStride];
    mCapacity = smStartCapacity;
  } else
  {
    char* oldData = mData;
    mCapacity = (int)((float)mCapacity * smGrowthFactor);
    char* newData = alloc char[mCapacity * mStride];
    Util::Copy<char>(oldData, newData, mSize * mStride);
    mData = newData;
    delete[] oldData;
  }
}

void Table::VerifyIndex(int index) const
{
  LogAbortIf(
    index >= mSize || index < 0, "Provided Table index is out of range.");
}

} // namespace Core
