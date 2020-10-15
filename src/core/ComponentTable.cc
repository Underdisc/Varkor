#include <iostream>

#include "debug/RedefNew.h"
#include "util/Memory.hh"
#include "ComponentTable.h"

const int ComponentTable::smStartCapacity = 10;
const float ComponentTable::smGrowthFactor = 2.0f;

void ComponentTable::Init(int stride)
{
  mData = nullptr;
  mStride = stride;
  mSize = 0;
  mCapacity = 0;
}

ComponentTable::~ComponentTable()
{
  if (mData != nullptr)
  {
    delete[] mData;
  }
}

int ComponentTable::Create(ObjRef object)
{
  if (mSize >= mCapacity)
  {
    Grow();
  }
  int componentIndex = mSize;
  ++mSize;
  mOwners.Push(object);
  mInUse.Push(true);
  return componentIndex;
}

void* ComponentTable::operator[](int index)
{
  return (void*)(mData + (mStride * index));
}

void ComponentTable::ShowStats()
{
  std::cout << "Head: " << (mData != nullptr) << std::endl
            << "Stride: " << mStride << std::endl
            << "Size: " << mSize << std::endl
            << "SizeInBytes: " << mSize * mStride << std::endl
            << "Capacity: " << mCapacity << std::endl
            << "CapacityInBytes: " << mCapacity * mStride << std::endl;
}

void ComponentTable::Grow()
{
  if (mData == nullptr)
  {
    mData = new char[smStartCapacity * mStride];
    mCapacity = smStartCapacity;
  } else
  {
    char* oldData = mData;
    mCapacity = (int)((float)mCapacity * smGrowthFactor);
    char* newData = new char[mCapacity * mStride];
    Util::Copy<char>(oldData, newData, mSize * mStride);
    mData = newData;
    delete[] oldData;
  }
}
