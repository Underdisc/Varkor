#ifndef math_Complex_h
#define math_Complex_h

namespace Math {

template<typename T>
class Complex
{
public:
  T mReal;
  T mImaginary;

public:
  Complex<T>& operator=(const Complex<T>& other);
  Complex<T> operator+(const Complex<T>& other) const;
  Complex<T> operator-(const Complex<T>& other) const;
  Complex<T> operator*(const Complex<T>& other) const;
  Complex<T> operator*(T scaler) const;
  Complex<T>& operator+=(const Complex<T>& other);
  Complex<T>& operator-=(const Complex<T>& other);
  Complex<T>& operator*=(const Complex<T>& other);
  Complex<T>& operator*=(T scaler);
};

Complex<float> ComplexPolar(float magnitude, float angle);

} // namespace Math

typedef Math::Complex<float> Complex;

#include "Complex.hh"

#endif
