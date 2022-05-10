#include <sstream>

#include "Error.h"
#include "debug/MemLeak.h"
#include "util/Memory.h"
#include "util/Utility.h"

namespace Ds {

template<typename T>
const size_t Vector<T>::smStartCapacity = 10;
template<typename T>
const float Vector<T>::smGrowthFactor = 2.0f;

template<typename T>
Vector<T>::Vector(): mData(nullptr), mSize(0), mCapacity(0)
{}

template<typename T>
Vector<T>::Vector(const Vector<T>& other):
  mSize(other.mSize), mCapacity(other.mSize)
{
  if (other.mData == nullptr) {
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
  if (mData != nullptr) {
    DeleteAllocation(mData);
  }
}

template<typename T>
void Vector<T>::Push(const T& value)
{
  if (mSize >= mCapacity) {
    Grow();
  }
  new (mData + mSize) T(value);
  ++mSize;
}

template<typename T>
void Vector<T>::Push(T&& value)
{
  if (mSize >= mCapacity) {
    Grow();
  }
  new (mData + mSize) T(Util::Forward(value));
  ++mSize;
}

template<typename T>
void Vector<T>::Push(const T& value, size_t count)
{
  size_t newSize = mSize + count;
  size_t newCapacity = mCapacity;
  if (newCapacity == 0) {
    newCapacity = smStartCapacity;
  }
  while (newSize > newCapacity) {
    newCapacity = (size_t)((float)newCapacity * smGrowthFactor);
  }
  if (newCapacity > mCapacity) {
    Grow(newCapacity);
  }
  Util::Fill<T>(mData + mSize, value, count);
  mSize = newSize;
}

template<typename T>
template<typename... Args>
void Vector<T>::Emplace(Args&&... args)
{
  if (mSize >= mCapacity) {
    Grow();
  }
  new (mData + mSize) T(Util::Forward(args)...);
  ++mSize;
}

template<typename T>
void Vector<T>::Insert(size_t index, const T& value)
{
  if (index == mSize) {
    Push(value);
    return;
  }
  VerifyIndex(index);

  if (mSize >= mCapacity) {
    Grow();
  }
  new (mData + mSize) T();
  for (size_t i = mSize; i > index; --i) {
    mData[i] = Util::Move(mData[i - 1]);
  }
  mData[index] = value;
  ++mSize;
}

template<typename T>
void Vector<T>::Pop()
{
  if (mSize != 0) {
    mData[mSize - 1].~T();
    --mSize;
  }
}

template<typename T>
void Vector<T>::Clear()
{
  for (size_t i = 0; i < mSize; ++i) {
    mData[i].~T();
  }
  mSize = 0;
}

template<typename T>
void Vector<T>::Remove(size_t index)
{
  VerifyIndex(index);
  mData[index].~T();
  for (size_t i = index + 1; i < mSize; ++i) {
    mData[i - 1] = Util::Move(mData[i]);
  }
  --mSize;
}

template<typename T>
void Vector<T>::LazyRemove(size_t index)
{
  VerifyIndex(index);
  if (mSize == 1) {
    Pop();
    return;
  }
  mData[index].~T();
  mData[index] = Util::Move(mData[mSize - 1]);
  --mSize;
}

template<typename T>
template<typename... Args>
void Vector<T>::Resize(size_t newSize, Args&&... args)
{
  for (size_t i = newSize; i < mSize; ++i) {
    mData[i].~T();
  }
  if (newSize > mCapacity) {
    Grow(newSize);
  }
  for (size_t i = mSize; i < newSize; ++i) {
    new (mData + i) T(args...);
  }
  mSize = newSize;
}

template<typename T>
void Vector<T>::Reserve(size_t newCapacity)
{
  if (mCapacity >= newCapacity) {
    return;
  }
  Grow(newCapacity);
}

template<typename T>
void Vector<T>::Shrink()
{
  if (mSize == mCapacity) {
    return;
  }

  T* oldData = mData;
  mData = CreateAllocation(mSize);
  Util::MoveData(oldData, mData, mSize);
  DeleteAllocation(oldData);
  mCapacity = mSize;
}

template<typename T>
size_t Vector<T>::Find(const T& value) const
{
  for (size_t i = 0; i < mSize; ++i) {
    if (mData[i] == value) {
      return i;
    }
  }
  return mSize;
}

template<typename T>
bool Vector<T>::Contains(const T& value) const
{
  size_t index = Find(value);
  if (index == mSize) {
    return false;
  }
  return true;
}

template<typename T>
size_t Vector<T>::Size() const
{
  return mSize;
}

template<typename T>
bool Vector<T>::Empty() const
{
  return mSize == 0;
}

template<typename T>
size_t Vector<T>::Capacity() const
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
  LogAbortIf(mSize == 0, "The Vector is empty.");
  return mData[mSize - 1];
}

template<typename T>
const T& Vector<T>::operator[](size_t index) const
{
  VerifyIndex(index);
  return mData[index];
}

template<typename T>
T& Vector<T>::operator[](size_t index)
{
  VerifyIndex(index);
  return mData[index];
}

template<typename T>
Vector<T>& Vector<T>::operator=(const Vector<T>& other)
{
  Clear();
  if (other.mSize <= mCapacity) {
    Util::Copy<T>(other.mData, mData, mSize);
    mSize = other.mSize;
    return *this;
  }

  if (mData != nullptr) {
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
  if (mData != nullptr) {
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
void Vector<T>::VerifyIndex(size_t index) const
{
  if (index < 0 || index >= mSize) {
    std::stringstream error;
    error << index << " is not a valid index.";
    LogAbort(error.str().c_str());
  }
}

template<typename T>
void Vector<T>::Grow()
{
  if (mData == nullptr) {
    mCapacity = smStartCapacity;
    mData = CreateAllocation(mCapacity);
  }
  else {
    size_t newCapacity = (size_t)((float)mCapacity * smGrowthFactor);
    Grow(newCapacity);
  }
}

template<typename T>
void Vector<T>::Grow(size_t newCapacity)
{
  LogAbortIf(
    newCapacity <= mCapacity,
    "The new capacity must be greater than the current capacity.");

  T* newData = CreateAllocation(newCapacity);
  for (size_t i = 0; i < mSize; ++i) {
    new (newData + i) T(Util::Move(mData[i]));
    mData[i].~T();
  }
  mCapacity = newCapacity;
  DeleteAllocation(mData);
  mData = newData;
}

template<typename T>
T* Vector<T>::CreateAllocation(size_t capacity)
{
  size_t byteCount = sizeof(T) * capacity;
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
