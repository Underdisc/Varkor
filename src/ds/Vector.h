#ifndef ds_Vector_h
#define ds_Vector_h

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

} // namespace DS

#include "Vector.hh"

#endif
