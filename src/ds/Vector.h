#ifndef ds_Vector_h
#define ds_Vector_h

namespace Ds {

template<typename T>
class Vector
{
public:
  Vector();
  Vector(const Vector<T>& other);
  Vector(Vector<T>&& other);
  ~Vector();
  void Push(const T& value);
  void Push(T&& value);
  void Push(const T& value, int count);
  template<typename... Args>
  void Emplace(const Args&... args);
  void Insert(int index, const T& value);
  void Pop();
  void Clear();
  void LazyRemove(int index);
  void Resize(int newSize);
  void Resize(int newSize, const T& fill);
  void Reserve(int newCapacity);
  void Shrink();

  bool Contains(const T& value) const;
  int Size() const;
  int Capacity() const;
  const T* CData() const;
  T& Top() const;

  const T& operator[](int index) const;
  T& operator[](int index);
  Vector<T>& operator=(const Vector<T>& other);
  Vector<T>& operator=(Vector<T>&& other);

  T* begin();
  T* end();
  const T* begin() const;
  const T* end() const;

  const static int smStartCapacity;
  const static float smGrowthFactor;

private:
  T* mData;
  int mSize;
  int mCapacity;

private:
  void VerifyIndex(int index) const;
  void Grow();
  void Grow(int newCapacity);
  void HandleNewSize(int newSize);
  T* CreateAllocation(int capacity);
  void DeleteAllocation(T* allocation);
};

} // namespace Ds

#include "Vector.hh"

#endif
