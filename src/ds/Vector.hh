#include "Error.h"
#include "debug/MemLeak.h"
#include "util/Memory.hh"

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
  mData(alloc T[other.mSize]), mSize(other.mSize), mCapacity(other.mSize)
{
  Util::Copy<T>(other.mData, mData, other.mSize);
}

template<typename T>
Vector<T>::~Vector()
{
  if (mData != nullptr)
  {
    delete[] mData;
  }
}

template<typename T>
void Vector<T>::Push(const T& value)
{
  if (mSize >= mCapacity)
  {
    Grow();
  }
  mData[mSize] = value;
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
void Vector<T>::Pop()
{
  if (mSize != 0)
  {
    mData[mSize].~T();
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
    mData = alloc T[newSize];
    Util::Copy<T>(oldData, mData, mSize);
    delete[] oldData;
  }
  int sizeDifference = newSize - mSize;
  Util::Fill<T>(mData + mSize, value, sizeDifference);
  mCapacity = newSize;
  mSize = newSize;
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
    mData = alloc T[mCapacity];
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
  mData = alloc T[newCapacity];
  Util::Copy<T>(oldData, mData, mSize);
  mCapacity = newCapacity;
  delete[] oldData;
}

} // namespace Ds
