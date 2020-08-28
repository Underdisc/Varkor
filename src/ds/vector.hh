#ifndef DS_Vector_hh
#define DS_Vector_hh

#include <iostream>

namespace DS {

template<typename T>
class Vector
{
public:
  T* _data;

public:
  Vector();
  ~Vector();
  void Push(const T& value);
  void Pop();
  void Clear();
  int Size() const;
  int Capacity() const;
  const T& operator[](int index) const;
  T& operator[](int index);

private:
  int _size;
  int _capacity;
  const static int _start_capacity;
  const static float _growth_factor;

private:
  void Grow();
  void Copy(T* from, T* to, int amount);
};

template<typename T>
const int Vector<T>::_start_capacity = 10;
template<typename T>
const float Vector<T>::_growth_factor = 2.0f;

template<typename T>
Vector<T>::Vector()
{
  _data = nullptr;
  _size = 0;
  _capacity = 0;
}

template<typename T>
Vector<T>::~Vector()
{
  if (_data != nullptr)
  {
    delete[] _data;
  }
}

template<typename T>
void Vector<T>::Push(const T& value)
{
  if (_size >= _capacity)
  {
    Grow();
  }
  _data[_size] = value;
  ++_size;
}

template<typename T>
void Vector<T>::Pop()
{
  if (_size != 0)
  {
    --_size;
  }
}

template<typename T>
void Vector<T>::Clear()
{
  _size = 0;
}

template<typename T>
int Vector<T>::Size() const
{
  return _size;
}

template<typename T>
int Vector<T>::Capacity() const
{
  return _capacity;
}

template<typename T>
const T& Vector<T>::operator[](int index) const
{
  return _data[index];
}

template<typename T>
T& Vector<T>::operator[](int index)
{
  return _data[index];
}

template<typename T>
void Vector<T>::Grow()
{
  if (_data == nullptr)
  {
    _capacity = _start_capacity;
    _data = new T[_capacity];
  } else
  {
    T* _oldData = _data;
    _capacity = (int)((float)_capacity * _growth_factor);
    _data = new T[_capacity];
    Copy(_oldData, _data, _size);
    delete[] _oldData;
  }
}

template<typename T>
void Vector<T>::Copy(T* from, T* to, int amount)
{
  for (int i = 0; i < amount; ++i)
  {
    to[i] = from[i];
  }
}

} // namespace DS

#endif
