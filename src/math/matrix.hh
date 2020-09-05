#ifndef math_matrix_h
#define math_matrix_h

// todo: These should not be included with this template header file.
#include <iomanip>
#include <iostream>
#include <sstream>

#include "../error.h"
#include "vector.hh"

namespace Math {

template<typename T, unsigned int N>
class Matrix
{
public:
  void Clear();
  void Identity();
  void Scale(const Vector<T, N - 1>& scale);
  void Translate(const Vector<T, N - 1>& translation);
  Matrix<T, N> Inverse() const;
  void MultiplyRow(int row, T factor);
  void AddRowMultiple(int row, int factor_row, T factor);
  void SwapRows(int row_a, int row_b);
  T* Data();
  T* operator[](int row);
  Matrix<T, N>& operator=(const Matrix<T, N>& other);
  Matrix<T, N> operator*(const Matrix<T, N>& other);
  Matrix<T, N>& operator*=(const Matrix<T, N>& other);

private:
  T CalculateProductElement(
    int element_row, int element_column, const Matrix<T, N>& other);
  T _value[N][N];
};

template<typename T, unsigned int N>
void Matrix<T, N>::Clear()
{
  for (int r = 0; r < N; ++r)
  {
    for (int c = 0; c < N; ++c)
    {
      _value[r][c] = (T)0;
    }
  }
}

template<typename T, unsigned int N>
void Matrix<T, N>::Identity()
{
  Clear();
  for (int d = 0; d < N; ++d)
  {
    _value[d][d] = (T)1;
  }
}

template<typename T, unsigned int N>
void Matrix<T, N>::Scale(const Vector<T, N - 1>& scale)
{
  Clear();
  for (int i = 0; i < N - 1; ++i)
  {
    _value[i][i] = scale[i];
  }
  _value[N - 1][N - 1] = (T)1;
}

template<typename T, unsigned int N>
void Matrix<T, N>::Translate(const Vector<T, N - 1>& translate)
{
  Identity();
  for (int i = 0; i < N - 1; ++i)
  {
    _value[i][N - 1] = translate[i];
  }
}

template<typename T, unsigned int N>
Matrix<T, N> Matrix<T, N>::Inverse() const
{
  // We find the inverse using Gauss-Jordan elimination. That is, we augment
  // the square matrix by the identity and perform row operations until the new
  // N by 2N matrix is in reduced row echelon form. At this point the inverse
  // matrix is what used to be the identity after the augmentation.

  // These two matrices represent the N by 2N matrix together. We will
  // perform the same row operations on both of these matrices.
  Matrix<T, N> copy = *this;
  Matrix<T, N> inverse;
  inverse.Identity();
  for (int pivot = 0; pivot < N; ++pivot)
  {
    // If the value of this pivot is zero, we need to swap the current pivot row
    // with a row that has a value in current pivot column that is non zero.
    if (copy[pivot][pivot] == 0)
    {
      bool still_invertible = false;
      for (int row = pivot + 1; row < N; ++row)
      {
        if (copy[row][pivot] != (T)0)
        {
          copy.SwapRows(row, pivot);
          inverse.SwapRows(row, pivot);
          still_invertible = true;
          break;
        }
      }
      // If we can't find a non zero value below the pivot, the matrix is not
      // invertible.
      LogAbortIf(!still_invertible, "The matrix is not invertible.");
    }

    // We change the pivot to one and make all values below the pivot zero using
    // row operations.
    T row_factor = (T)1 / copy[pivot][pivot];
    copy.MultiplyRow(pivot, row_factor);
    inverse.MultiplyRow(pivot, row_factor);
    for (int row = pivot + 1; row < N; ++row)
    {
      row_factor = copy[row][pivot] * (T)-1;
      copy.AddRowMultiple(row, pivot, row_factor);
      inverse.AddRowMultiple(row, pivot, row_factor);
    }
  }

  // We now force all elements that are above pivots to be zeros using row
  // operations.
  for (int pivot = 1; pivot < N; ++pivot)
  {
    for (int row = pivot - 1; row >= 0; --row)
    {
      T row_factor = copy[row][pivot] * (T)-1;
      copy.AddRowMultiple(row, pivot, row_factor);
      inverse.AddRowMultiple(row, pivot, row_factor);
    }
  }
  return inverse;
}

template<typename T, unsigned int N>
void Matrix<T, N>::MultiplyRow(int row, T factor)
{
  for (int col = 0; col < N; ++col)
  {
    _value[row][col] *= factor;
  }
}

template<typename T, unsigned int N>
void Matrix<T, N>::AddRowMultiple(int row, int factor_row, T factor)
{
  for (int col = 0; col < N; ++col)
  {
    _value[row][col] += _value[factor_row][col] * factor;
  }
}

template<typename T, unsigned int N>
void Matrix<T, N>::SwapRows(int row_a, int row_b)
{
  for (int col = 0; col < N; ++col)
  {
    T temp = _value[row_a][col];
    _value[row_a][col] = _value[row_b][col];
    _value[row_b][col] = temp;
  }
}

template<typename T, unsigned int N>
T* Matrix<T, N>::Data()
{
  return _value[0];
}

template<typename T, unsigned int N>
T* Matrix<T, N>::operator[](int row)
{
  return _value[row];
}

template<typename T, unsigned int N>
Matrix<T, N>& Matrix<T, N>::operator=(const Matrix<T, N>& other)
{
  for (int r = 0; r < N; ++r)
  {
    for (int c = 0; c < N; ++c)
    {
      _value[r][c] = other._value[c][r];
    }
  }
  return *this;
}

template<typename T, unsigned int N>
Matrix<T, N> Matrix<T, N>::operator*(const Matrix<T, N>& other)
{
  Matrix<T, N> result;
  for (int r = 0; r < N; ++r)
  {
    for (int c = 0; c < N; ++c)
    {
      result[r][c] = CalculateProductElement(r, c, other);
    }
  }
  return result;
}

template<typename T, unsigned int N>
Matrix<T, N>& Matrix<T, N>::operator*=(const Matrix<T, N>& other)
{
  Matrix<T, N> result = *this * other;
  *this = result;
  return *this;
}

template<typename T, unsigned int N>
T Matrix<T, N>::CalculateProductElement(
  int element_row, int element_column, const Matrix<T, N>& other)
{
  T result = (T)0;
  for (int i = 0; i < N; ++i)
  {
    result += _value[element_row][i] * other._value[i][element_column];
  }
  return result;
}

template<typename T, unsigned int N>
std::ostream& operator<<(std::ostream& os, Matrix<T, N> mat)
{
  // We find the longest number in each column so that all columns are aligned
  // when printed to console.
  int longest_widths[N];
  for (int c = 0; c < N; ++c)
  {
    longest_widths[c] = 1;
    for (int r = 0; r < N; ++r)
    {
      std::stringstream ss;
      ss << mat[r][c];
      int width = ss.str().size();
      if (width > longest_widths[c])
      {
        longest_widths[c] = width;
      }
    }
  }

  for (int r = 0; r < N; ++r)
  {
    os << "[";
    for (int c = 0; c < N; ++c)
    {
      os << std::setw(longest_widths[c]) << std::right << mat[r][c];
      if (c < N - 1)
      {
        os << ", ";
      }
    }
    os << "]" << std::endl;
  }
  return os;
}

} // namespace Math

typedef Math::Matrix<float, 4> Mat4;

#endif