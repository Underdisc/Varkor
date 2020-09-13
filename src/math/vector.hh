#ifndef math_vector_hh
#define math_vector_hh

#include <iostream>
#include <math.h>

#include "../error.h"

namespace Math {

// todo: separate the declaration from the definition.
template<typename T, unsigned int N>
class Vector
{
public:
  T mValue[N];

public:
  T operator[](int index) const;
  T& operator[](int index);
  Vector<T, N>& operator=(const Vector<T, N>& other);
  Vector<T, N> operator+(const Vector<T, N>& other) const;
  Vector<T, N> operator-(const Vector<T, N>& other) const;
  Vector<T, N> operator*(const Vector<T, N>& other) const;
  Vector<T, N> operator*(T scalar) const;
  Vector<T, N> operator/(T scalar) const;
  Vector<T, N>& operator+=(const Vector<T, N>& other);
  Vector<T, N>& operator-=(const Vector<T, N>& other);
  Vector<T, N>& operator*=(const Vector<T, N>& other);
  Vector<T, N>& operator*=(T scalar);
  Vector<T, N>& operator/=(T scalar);
  Vector<T, N> operator-();
  template<unsigned int M>
  operator Vector<T, M>() const;
};

template<typename T, unsigned int N>
T MagSq(const Vector<T, N>& vec);
template<unsigned int N>
float Mag(const Vector<float, N>& vec);
template<typename T, unsigned int N>
Vector<T, N> Normalize(const Vector<T, N>& vec);
template<typename T>
Vector<T, 3> Cross(const Vector<T, 3>& a, const Vector<T, 3>& b);
template<typename T, unsigned int N>
T Dot(const Vector<T, N>& a, const Vector<T, N>& b);

template<typename T, unsigned int N>
T Vector<T, N>::operator[](int index) const
{
  return mValue[index];
}

template<typename T, unsigned int N>
T& Vector<T, N>::operator[](int index)
{
  return mValue[index];
}

template<typename T, unsigned int N>
Vector<T, N>& Vector<T, N>::operator=(const Vector<T, N>& other)
{
  for (int i = 0; i < N; ++i)
  {
    mValue[i] = other[i];
  }
  return *this;
}

template<typename T, unsigned int N>
Vector<T, N> Vector<T, N>::operator+(const Vector<T, N>& other) const
{
  Vector<T, N> result;
  for (int i = 0; i < N; ++i)
  {
    result[i] = mValue[i] + other[i];
  }
  return result;
}

template<typename T, unsigned int N>
Vector<T, N> Vector<T, N>::operator-(const Vector<T, N>& other) const
{
  Vector<T, N> result;
  for (int i = 0; i < N; ++i)
  {
    result[i] = mValue[i] - other[i];
  }
  return result;
}

template<typename T, unsigned int N>
Vector<T, N> Vector<T, N>::operator*(const Vector<T, N>& other) const
{
  Vector<T, N> result;
  for (int i = 0; i < N; ++i)
  {
    result[i] = mValue[i] * other[i];
  }
  return result;
}

template<typename T, unsigned int N>
Vector<T, N> Vector<T, N>::operator*(T scalar) const
{
  Vector<T, N> result;
  for (int i = 0; i < N; ++i)
  {
    result[i] = mValue[i] * scalar;
  }
  return result;
}

template<typename T, unsigned int N>
Vector<T, N> Vector<T, N>::operator/(T scalar) const
{
  Vector<T, N> result;
  for (int i = 0; i < N; ++i)
  {
    result[i] = mValue[i] / scalar;
  }
  return result;
}

template<typename T, unsigned int N>
Vector<T, N>& Vector<T, N>::operator+=(const Vector<T, N>& other)
{
  for (int i = 0; i < N; ++i)
  {
    mValue[i] += other[i];
  }
  return *this;
}

template<typename T, unsigned int N>
Vector<T, N>& Vector<T, N>::operator-=(const Vector<T, N>& other)
{
  for (int i = 0; i < N; ++i)
  {
    mValue[i] -= other[i];
  }
  return *this;
}

template<typename T, unsigned int N>
Vector<T, N>& Vector<T, N>::operator*=(const Vector<T, N>& other)
{
  for (int i = 0; i < N; ++i)
  {
    mValue[i] *= other[i];
  }
  return *this;
}

template<typename T, unsigned int N>
Vector<T, N>& Vector<T, N>::operator*=(T scalar)
{
  for (int i = 0; i < N; ++i)
  {
    mValue[i] *= scalar;
  }
  return *this;
}

template<typename T, unsigned int N>
Vector<T, N>& Vector<T, N>::operator/=(T scaler)
{
  for (int i = 0; i < N; ++i)
  {
    mValue[i] /= scaler;
  }
  return *this;
}

template<typename T, unsigned int N>
Vector<T, N> Vector<T, N>::operator-()
{
  Vector<T, N> negated;
  for (int i = 0; i < N; ++i)
  {
    negated[i] = -mValue[i];
  }
  return negated;
}

template<typename T, unsigned int N>
template<unsigned int M>
Vector<T, N>::operator Vector<T, M>() const
{
  Vector<T, M> result;
  for (int i = 0; i < N && i < M; ++i)
  {
    result.mValue[i] = mValue[i];
  }
  for (int i = M - 1; i >= N; --i)
  {
    result.mValue[i] = (T)0;
  }
  return result;
}

template<typename T, unsigned int N>
T MagSq(const Vector<T, N>& vec)
{
  return Dot(vec, vec);
}

template<unsigned int N>
float Mag(const Vector<float, N>& vec)
{
  return std::sqrtf(MagSq(vec));
}

template<typename T, unsigned int N>
Vector<T, N> Normalize(const Vector<T, N>& vec)
{
  T magnitude = Mag(vec);
  LogAbortIf(
    magnitude == (T)0, "Vector with magnitude of zero can't be normalized.");
  return vec / magnitude;
}

template<typename T>
Vector<T, 3> Cross(const Vector<T, 3>& a, const Vector<T, 3>& b)
{
  Vector<T, 3> cross;
  cross[0] = a[1] * b[2] - a[2] * b[1];
  cross[1] = a[2] * b[0] - a[0] * b[2];
  cross[2] = a[0] * b[1] - a[1] * b[0];
  return cross;
}

template<typename T, unsigned int N>
T Dot(const Vector<T, N>& a, const Vector<T, N>& b)
{
  T dot = (T)0;
  for (int i = 0; i < N; ++i)
  {
    dot += a[i] * b[i];
  }
  return dot;
}

template<typename T, unsigned int N>
std::ostream& operator<<(std::ostream& os, const Vector<T, N>& vec)
{
  os << "[";
  for (int i = 0; i < N; ++i)
  {
    os << vec[i];
    if (i < N - 1)
    {
      os << ", ";
    }
  }
  os << "]";
  return os;
}

} // namespace Math

typedef Math::Vector<float, 2> Vec2;
typedef Math::Vector<float, 3> Vec3;
typedef Math::Vector<float, 4> Vec4;

#endif