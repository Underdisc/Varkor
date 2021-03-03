#include <new>

#include "Error.h"
#include "debug/MemLeak.h"
#include "util/Memory.h"
#include "util/Utility.h"

namespace Ds {

template<typename T>
const int Vector<T>::smStartCapacity = 10;
template<typename T>
const float Vector<T>::smGrowthFactor = 2.0f;

template<typename T>
Vector<T>::Vector(): mData(nullptr), mSize(0), mCapacity(0)
{}

template<typename T>
Vector<T>::Vector(const Vector<T>& other):
  mSize(other.mSize), mCapacity(other.mSize)
{
  if (other.mData == nullptr)
  {
    mData = nullptr;
    return;
  }
  mData = CreateAllocation(other.mSize);
  Util::Copy<T>(other.mData, mData, other.mSize);
}

template<typename T>
Vector<T>::Vector(Vector<T>&& other):
  mData(other.mData), mSize(other.mSize), mCapacity(other.mCapacity)
{
  other.mData = nullptr;
  other.mSize = 0;
  other.mCapacity = 0;
}

template<typename T>
Vector<T>::~Vector()
{
  Clear();
  if (mData != nullptr)
  {
    DeleteAllocation(mData);
  }
}

template<typename T>
void Vector<T>::Push(const T& value)
{
  if (mSize >= mCapacity)
  {
    Grow();
  }
  new (mData + mSize) T(value);
  ++mSize;
}

template<typename T>
void Vector<T>::Push(T&& value)
{
  if (mSize >= mCapacity)
  {
    Grow();
  }
  new (mData + mSize) T(Util::Move(value));
  ++mSize;
}

template<typename T>
void Vector<T>::Push(const T& value, int count)
{
  int newSize = mSize + count;
  int newCapacity = mCapacity;
  if (newCapacity == 0)
  {
    newCapacity = smStartCapacity;
  }
  while (newSize > newCapacity)
  {
    newCapacity = (int)((float)newCapacity * smGrowthFactor);
  }
  if (newCapacity > mCapacity)
  {
    Grow(newCapacity);
  }
  Util::Fill<T>(mData + mSize, value, count);
  mSize = newSize;
}

template<typename T>
template<typename... Args>
void Vector<T>::Emplace(const Args&... args)
{
  if (mSize >= mCapacity)
  {
    Grow();
  }
  new (mData + mSize) T(args...);
  ++mSize;
}

template<typename T>
void Vector<T>::Insert(int index, const T& value)
{
  LogAbortIf(
    index > mSize || index < 0,
    "The provided index is outside of the range of insertable positions.");
  if (index == mSize)
  {
    Push(value);
    return;
  }

  if (mSize >= mCapacity)
  {
    Grow();
  }
  for (int i = mSize; i > index; --i)
  {
    mData[i] = Util::Move(mData[i - 1]);
  }
  mData[index] = value;
  ++mSize;
}

template<typename T>
void Vector<T>::Pop()
{
  if (mSize != 0)
  {
    mData[mSize - 1].~T();
    --mSize;
  }
}

template<typename T>
void Vector<T>::Clear()
{
  for (int i = 0; i < mSize; ++i)
  {
    mData[i].~T();
  }
  mSize = 0;
}

template<typename T>
void Vector<T>::Resize(int newSize, const T& value)
{
  if (newSize > mCapacity)
  {
    T* oldData = mData;
    mData = CreateAllocation(newSize);
    Util::MoveData<T>(oldData, mData, mSize);
    DeleteAllocation(oldData);
    mCapacity = newSize;
  }
  int sizeDifference = newSize - mSize;
  Util::Fill<T>(mData + mSize, value, sizeDifference);
  mSize = newSize;
}

template<typename T>
void Vector<T>::Reserve(int newCapacity)
{
  if (mCapacity >= newCapacity)
  {
    return;
  }
  Grow(newCapacity);
}

template<typename T>
void Vector<T>::Shrink()
{
  if (mSize == mCapacity)
  {
    return;
  }

  T* oldData = mData;
  mData = CreateAllocation(mSize);
  Util::MoveData(oldData, mData, mSize);
  DeleteAllocation(oldData);
  mCapacity = mSize;
}

template<typename T>
bool Vector<T>::Contains(const T& value) const
{
  for (int i = 0; i < mSize; ++i)
  {
    if (mData[i] == value)
    {
      return true;
    }
  }
  return false;
}

template<typename T>
int Vector<T>::Size() const
{
  return mSize;
}

template<typename T>
int Vector<T>::Capacity() const
{
  return mCapacity;
}

template<typename T>
const T* Vector<T>::CData() const
{
  return mData;
}

template<typename T>
T& Vector<T>::Top() const
{
  return mData[mSize - 1];
}

template<typename T>
const T& Vector<T>::operator[](int index) const
{
  return mData[index];
}

template<typename T>
T& Vector<T>::operator[](int index)
{
  return mData[index];
}

template<typename T>
Vector<T>& Vector<T>::operator=(const Vector<T>& other)
{
  Clear();
  if (other.mSize <= mCapacity)
  {
    Util::Copy<T>(other.mData, mData, mSize);
    mSize = other.mSize;
    return *this;
  }

  if (mData != nullptr)
  {
    DeleteAllocation(mData);
  }
  mData = CreateAllocation(other.mSize);
  mSize = other.mSize;
  mCapacity = other.mSize;
  Util::Copy<T>(other.mData, mData, mSize);
  return *this;
}

template<typename T>
Vector<T>& Vector<T>::operator=(Vector<T>&& other)
{
  Clear();
  if (mData != nullptr)
  {
    DeleteAllocation(mData);
  }
  mData = other.mData;
  mSize = other.mSize;
  mCapacity = other.mCapacity;

  other.mData = nullptr;
  other.mSize = 0;
  other.mCapacity = 0;
  return *this;
}

template<typename T>
T* Vector<T>::begin()
{
  return mData;
}

template<typename T>
T* Vector<T>::end()
{
  return mData + mSize;
}

template<typename T>
const T* Vector<T>::begin() const
{
  return mData;
}

template<typename T>
const T* Vector<T>::end() const
{
  return mData + mSize;
}

template<typename T>
void Vector<T>::Grow()
{
  if (mData == nullptr)
  {
    mCapacity = smStartCapacity;
    mData = CreateAllocation(mCapacity);
  } else
  {
    int newCapacity = (int)((float)mCapacity * smGrowthFactor);
    Grow(newCapacity);
  }
}

template<typename T>
void Vector<T>::Grow(int newCapacity)
{
  LogAbortIf(
    newCapacity <= mCapacity,
    "The new capacity must be greater than the current capacity.");

  T* oldData = mData;
  mData = CreateAllocation(newCapacity);
  for (int i = 0; i < mSize; ++i)
  {
    new (mData + i) T(Util::Move(oldData[i]));
  }
  mCapacity = newCapacity;
  DeleteAllocation(oldData);
}

template<typename T>
T* Vector<T>::CreateAllocation(int capacity)
{
  int byteCount = sizeof(T) * capacity;
  char* byteAllocation = alloc char[byteCount];
  return (T*)byteAllocation;
}

template<typename T>
void Vector<T>::DeleteAllocation(T* allocation)
{
  char* byteAllocation = (char*)allocation;
  delete[] byteAllocation;
}

} // namespace Ds
