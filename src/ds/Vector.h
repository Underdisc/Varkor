#ifndef ds_Vector_h
#define ds_Vector_h

#include <cstdlib>

#include "Result.h"

namespace Ds {

template<typename T>
struct Vector {
public:
  Vector();
  Vector(const Vector<T>& other);
  Vector(Vector<T>&& other);
  Vector(const std::initializer_list<T>& other);
  ~Vector();
  void Push(const T& value);
  void Push(T&& value);
  void Push(const T& value, size_t count);
  template<typename... Args>
  void Emplace(Args&&... args);
  void Insert(size_t index, const T& value);
  void Insert(size_t index, T&& value);
  void Swap(size_t indexA, size_t indexB);
  void Sort();
  void Sort(bool (*greaterThan)(const T&, const T&));
  void Pop();
  void Clear();
  void Remove(size_t index);
  void LazyRemove(size_t index);
  template<typename... Args>
  void Resize(size_t newSize, Args&&... args);
  void Reserve(size_t newCapacity);
  void Shrink();

  template<typename CT>
  VResult<size_t> Find(const CT& value) const;
  template<typename CT>
  bool Contains(const CT& value) const;
  size_t Size() const;
  bool Empty() const;
  size_t Capacity() const;
  const T* CData() const;
  T* Data();
  T& Top() const;

  const T& operator[](size_t index) const;
  T& operator[](size_t index);
  Vector<T>& operator=(const Vector<T>& other);
  Vector<T>& operator=(Vector<T>&& other);
  Vector<T>& operator=(const std::initializer_list<T>& other);

  T* begin();
  T* end();
  const T* begin() const;
  const T* end() const;

  const static size_t smStartCapacity;
  const static float smGrowthFactor;

private:
  T* mData;
  size_t mSize;
  size_t mCapacity;

private:
  void Quicksort(int start, int end, bool (*greaterThan)(const T&, const T&));
  int Partition(int start, int end, bool (*greaterThan)(const T&, const T&));

  void VerifyIndex(size_t index) const;
  void CreateGap(size_t index);
  void Grow();
  void Grow(size_t newCapacity);
  T* CreateAllocation(size_t capacity);
  void DeleteAllocation(T* allocation);
};

} // namespace Ds

#include "Vector.hh"

#endif
