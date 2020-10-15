#ifndef ds_Vector_hh
#define ds_Vector_hh

#include "debug/RedefNew.h"
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
  void Pop();
  void Clear();
  bool Contains(const T& value) const;
  int Size() const;
  int Capacity() const;
  const T& operator[](int index) const;
  T& operator[](int index);

private:
  int mSize;
  int mCapacity;
  const static int smStartCapacity;
  const static float smGrowthFactor;

private:
  void Grow();
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
void Vector<T>::Grow()
{
  if (mData == nullptr)
  {
    mCapacity = smStartCapacity;
    mData = new T[mCapacity];
  } else
  {
    T* oldData = mData;
    mCapacity = (int)((float)mCapacity * smGrowthFactor);
    mData = new T[mCapacity];
    Util::Copy<T>(oldData, mData, mSize);
    delete[] oldData;
  }
}

} // namespace DS

#endif
