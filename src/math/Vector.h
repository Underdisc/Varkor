#ifndef math_Vector_h
#define math_Vector_h

#include <cstdlib>
#include <iostream>

namespace Math {

template<typename T, unsigned int N>
struct Vector {
  T mD[N];

  Vector() {}
  Vector(const Vector<T, N - 1>& a, T b);
  Vector(const std::initializer_list<T>& initList);
  Vector<T, N>& operator=(const std::initializer_list<T>& initList);
  static Vector<T, N> Zero();

  const T& operator[](int index) const;
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
Vector<T, N> operator/(T scalar, const Vector<T, N>& vector);
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
bool operator==(const Vector<T, N>& a, const Vector<T, N>& b);
template<typename T, unsigned int N>
bool Near(const Vector<T, N>& a, const Vector<T, N>& b);
template<typename T, unsigned int N>
bool Near(const Vector<T, N>& a, const Vector<T, N>& b, float epsilon);
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
template<typename T, unsigned int N>
T Angle(const Vector<T, N>& a, const Vector<T, N>& b);
template<typename T, unsigned int N>
Vector<T, N> ComponentwiseProduct(const Vector<T, N>& a, const Vector<T, N>& b);
template<typename T>
Vector<T, 3> PerpendicularTo(const Vector<T, 3>& vector);
template<typename T, unsigned int N>
Vector<T, N> ScaleToInterval(Vector<T, N> vector, float interval);
template<typename T, unsigned int N>
Vector<T, N> ScaleComponentsToInterval(Vector<T, N> vector, float interval);

} // namespace Math

#include "Vector.hh"

typedef Math::Vector<float, 2> Vec2;
typedef Math::Vector<float, 3> Vec3;
typedef Math::Vector<float, 4> Vec4;

#endif
