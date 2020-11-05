#ifndef math_Vector_h
#define math_Vector_h

#include <iostream>

namespace Math {

template<typename T, unsigned int N>
struct Vector
{
  T mD[N];

  T* Data();
  const T* CData() const;
  T operator[](int index) const;
  T& operator[](int index);
  template<unsigned int M>
  operator Vector<T, M>() const;
};

template<typename T, unsigned int N>
Vector<T, N> operator+(const Vector<T, N>& a, const Vector<T, N>& b);
template<typename T, unsigned int N>
Vector<T, N> operator-(const Vector<T, N>& a, const Vector<T, N>& b);
template<typename T, unsigned int N>
Vector<T, N> operator*(const Vector<T, N>& vector, T scalar);
template<typename T, unsigned int N>
Vector<T, N> operator*(T scaler, const Vector<T, N>& vector);
template<typename T, unsigned int N>
Vector<T, N> operator/(const Vector<T, N>& vector, T scalar);
template<typename T, unsigned int N>
Vector<T, N>& operator+=(Vector<T, N>& a, const Vector<T, N>& b);
template<typename T, unsigned int N>
Vector<T, N>& operator-=(Vector<T, N>& a, const Vector<T, N>& b);
template<typename T, unsigned int N>
Vector<T, N>& operator*=(Vector<T, N>& vector, T scalar);
template<typename T, unsigned int N>
Vector<T, N>& operator/=(Vector<T, N>& vector, T scalar);
template<typename T, unsigned int N>
Vector<T, N> operator-(const Vector<T, N>& vector);
template<typename T, unsigned int N>
std::ostream& operator<<(std::ostream& os, const Vector<T, N>& vec);

template<typename T, unsigned int N>
T MagnitudeSq(const Vector<T, N>& vector);
template<unsigned int N>
float Magnitude(const Vector<float, N>& vector);
template<typename T, unsigned int N>
Vector<T, N> Normalize(const Vector<T, N>& vector);
template<typename T>
Vector<T, 3> Cross(const Vector<T, 3>& a, const Vector<T, 3>& b);
template<typename T, unsigned int N>
T Dot(const Vector<T, N>& a, const Vector<T, N>& b);

} // namespace Math

#include "Vector.hh"

typedef Math::Vector<float, 2> Vec2;
typedef Math::Vector<float, 3> Vec3;
typedef Math::Vector<float, 4> Vec4;

#endif
