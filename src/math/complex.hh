#ifndef math_complex_hh
#define math_complex_hh

#include <iostream>
#include <math.h>

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

template<typename T>
Complex<T>& Complex<T>::operator=(const Complex<T>& other)
{
  mReal = other.mReal;
  mImaginary = other.mImaginary;
  return *this;
}

template<typename T>
Complex<T> Complex<T>::operator+(const Complex<T>& other) const
{
  Complex<T> result;
  result.mReal = mReal + other.mReal;
  result.mImaginary = mImaginary + other.mImaginary;
  return result;
}

template<typename T>
Complex<T> Complex<T>::operator-(const Complex<T>& other) const
{
  Complex<T> result;
  result.mReal = mReal - other.mReal;
  result.mImaginary = mImaginary - other.mImaginary;
  return result;
}

template<typename T>
Complex<T> Complex<T>::operator*(const Complex<T>& other) const
{
  Complex<T> result;
  result.mReal = mReal * other.mReal - mImaginary * other.mImaginary;
  result.mImaginary = mReal * other.mImaginary + mImaginary * other.mReal;
  return result;
}

template<typename T>
Complex<T> Complex<T>::operator*(T scaler) const
{
  Complex<T> result;
  result.mReal = mReal * scaler;
  result.mImaginary = mImaginary * scaler;
  return result;
}

template<typename T>
Complex<T>& Complex<T>::operator+=(const Complex<T>& other)
{
  mReal += other.mReal;
  mImaginary += other.mImaginary;
  return *this;
}

template<typename T>
Complex<T>& Complex<T>::operator-=(const Complex<T>& other)
{
  mReal -= other.mReal;
  mImaginary -= other.mImaginary;
  return *this;
}

template<typename T>
Complex<T>& Complex<T>::operator*=(const Complex<T>& other)
{
  T newReal = mReal * other.mReal - mImaginary * other.mImaginary;
  T newImaginary = mReal * other.mImaginary + mImaginary * other.mReal;
  mReal = newReal;
  mImaginary = newImaginary;
  return *this;
}

template<typename T>
Complex<T>& Complex<T>::operator*=(T scaler)
{
  mReal = mReal * scaler;
  mImaginary = mImaginary * scaler;
  return *this;
}

Complex<float> ComplexPolar(float magnitude, float angle)
{
  Complex<float> result;
  result.mReal = magnitude * std::cosf(angle);
  result.mImaginary = magnitude * std::sinf(angle);
  return result;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Complex<T>& value)
{
  os << value.mReal;
  if (value.mImaginary >= (T)0)
  {
    os << "+";
  }
  os << value.mImaginary << "i";
  return os;
}

} // namespace Math

typedef Math::Complex<float> Complex;

#endif
