#ifndef ds_Vector_hh
#define ds_Vector_hh

#include "Error.h"
#include "debug/MemLeak.h"
#include "util/Memory.hh"

namespace DS {

template<typename T>
class Vector
{
public:
  T* mData;

public:
  Vector();
  ~Vector();
  void Push(const T& value);
  void Push(const T& value, int count);
  void Pop();
  void Clear();
  void Resize(int newSize, const T& fill);
  bool Contains(const T& value) const;
  int Size() const;
  int Capacity() const;
  const T& operator[](int index) const;
  T& operator[](int index);
  T* begin();
  T* end();
  const T* begin() const;
  const T* end() const;

private:
  int mSize;
  int mCapacity;
  const static int smStartCapacity;
  const static float smGrowthFactor;

private:
  void Grow();
  void Grow(int newCapacity);
};

template<typename T>
const int Vector<T>::smStartCapacity = 10;
template<typename T>
const float Vector<T>::smGrowthFactor = 2.0f;

template<typename T>
Vector<T>::Vector()
{
  mData = nullptr;
  mSize = 0;
  mCapacity = 0;
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
void Vector<T>::Pop()
{
  if (mSize != 0)
  {
    --mSize;
  }
}

template<typename T>
void Vector<T>::Clear()
{
  mSize = 0;
}

template<typename T>
void Vector<T>::Resize(int newSize, const T& value)
{
  if (newSize > mCapacity)
  {
    T* oldData = mData;
    mData = new T[newSize];
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
    mData = new T[mCapacity];
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
  mData = new T[newCapacity];
  Util::Copy<T>(oldData, mData, mSize);
  mCapacity = newCapacity;
  delete[] oldData;
}

} // namespace DS

#endif
