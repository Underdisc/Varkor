#ifndef ds_Vector_h
#define ds_Vector_h

namespace Ds {

template<typename T>
class Vector
{
public:
  Vector();
  Vector(const Vector<T>& other);
  ~Vector();
  void Push(const T& value);
  void Push(const T& value, int count);
  void Pop();
  void Clear();
  void Resize(int newSize, const T& fill);
  template<typename... Args>
  void Emplace(const Args&... args);

  bool Contains(const T& value) const;
  int Size() const;
  int Capacity() const;
  const T* CData() const;
  T& Top() const;

  const T& operator[](int index) const;
  T& operator[](int index);
  Vector<T>& operator=(const Vector<T>& other);

  T* begin();
  T* end();
  const T* begin() const;
  const T* end() const;

private:
  T* mData;
  int mSize;
  int mCapacity;
  const static int smStartCapacity;
  const static float smGrowthFactor;

private:
  void Grow();
  void Grow(int newCapacity);
};

} // namespace Ds

#include "Vector.hh"

#endif
