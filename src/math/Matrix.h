#ifndef math_Matrix_h
#define math_Matrix_h

#include <iostream>

#include "math/Vector.h"

namespace Math {

template<typename T, unsigned int N>
struct Matrix
{
  T mD[N][N];

  T* Data();
  const T* CData() const;
  T* operator[](int row);
  const T* operator[](int row) const;
};

template<typename T, unsigned int N>
Matrix<T, N> operator*(const Matrix<T, N>& a, const Matrix<T, N>& b);
template<typename T, unsigned int N>
Matrix<T, N>& operator*=(Matrix<T, N>& a, const Matrix<T, N>& b);
template<typename T, unsigned int N>
Vector<T, N> operator*(const Matrix<T, N>& matrix, const Vector<T, N>& vector);
template<typename T, unsigned int N>
std::ostream& operator<<(std::ostream& os, const Matrix<T, N>& matrix);

template<typename T, unsigned int N>
void Zero(Matrix<T, N>* matrix);
template<typename T, unsigned int N>
void Identity(Matrix<T, N>* matrix);
template<typename T, unsigned int N>
void Scale(Matrix<T, N>* matrix, const Vector<T, N - 1>& scale);
template<typename T, unsigned int N>
void Translate(Matrix<T, N>* matrix, const Vector<T, N - 1>& translation);
template<typename T, unsigned int N>
Vector<T, N - 1> ApplyToPoint(
  const Matrix<T, N>& matrix, const Vector<T, N - 1>& point);
template<typename T, unsigned int N>
Vector<T, N - 1> ApplyToVector(
  const Matrix<T, N>& matrix, const Vector<T, N - 1>& vector);

template<typename T, unsigned int N>
Matrix<T, N> HomogeneousOrthogonalInverse(const Matrix<T, N>& matrix);
template<typename T, unsigned int N>
Matrix<T, N> Inverse(const Matrix<T, N>& matrix);
template<typename T, unsigned int N>
void MultiplyRow(Matrix<T, N>* matrix, int row, T factor);
template<typename T, unsigned int N>
void AddRowMultiple(Matrix<T, N>* matrix, int row, int factorRow, T factor);
template<typename T, unsigned int N>
void SwapRows(Matrix<T, N>* matrix, int rowA, int rowB);
template<typename T, unsigned int N>
T CalculateProductElement(
  const Matrix<T, N>& a,
  const Matrix<T, N>& b,
  int elementRow,
  int elementColumn);

} // namespace Math

#include "Matrix.hh"

#endif
