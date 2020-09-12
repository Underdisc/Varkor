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
  void AddRowMultiple(int row, int factorRow, T factor);
  void SwapRows(int rowA, int rowB);
  T* Data();
  const T* CData() const;
  T* operator[](int row);
  Matrix<T, N>& operator=(const Matrix<T, N>& other);
  Matrix<T, N> operator*(const Matrix<T, N>& other) const;
  Matrix<T, N>& operator*=(const Matrix<T, N>& other);

private:
  T CalculateProductElement(
    int elementRow, int elementColumn, const Matrix<T, N>& other) const;
  T mValue[N][N];
};

template<typename T, unsigned int N>
void Matrix<T, N>::Clear()
{
  for (int r = 0; r < N; ++r)
  {
    for (int c = 0; c < N; ++c)
    {
      mValue[r][c] = (T)0;
    }
  }
}

template<typename T, unsigned int N>
void Matrix<T, N>::Identity()
{
  Clear();
  for (int d = 0; d < N; ++d)
  {
    mValue[d][d] = (T)1;
  }
}

template<typename T, unsigned int N>
void Matrix<T, N>::Scale(const Vector<T, N - 1>& scale)
{
  Clear();
  for (int i = 0; i < N - 1; ++i)
  {
    mValue[i][i] = scale[i];
  }
  mValue[N - 1][N - 1] = (T)1;
}

template<typename T, unsigned int N>
void Matrix<T, N>::Translate(const Vector<T, N - 1>& translate)
{
  Identity();
  for (int i = 0; i < N - 1; ++i)
  {
    mValue[i][N - 1] = translate[i];
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
      bool stillInvertible = false;
      for (int row = pivot + 1; row < N; ++row)
      {
        if (copy[row][pivot] != (T)0)
        {
          copy.SwapRows(row, pivot);
          inverse.SwapRows(row, pivot);
          stillInvertible = true;
          break;
        }
      }
      // If we can't find a non zero value below the pivot, the matrix is not
      // invertible.
      LogAbortIf(!stillInvertible, "The matrix is not invertible.");
    }

    // We change the pivot to one and make all values below the pivot zero using
    // row operations.
    T rowFactor = (T)1 / copy[pivot][pivot];
    copy.MultiplyRow(pivot, rowFactor);
    inverse.MultiplyRow(pivot, rowFactor);
    for (int row = pivot + 1; row < N; ++row)
    {
      rowFactor = copy[row][pivot] * (T)-1;
      copy.AddRowMultiple(row, pivot, rowFactor);
      inverse.AddRowMultiple(row, pivot, rowFactor);
    }
  }

  // We now force all elements that are above pivots to be zeros using row
  // operations.
  for (int pivot = 1; pivot < N; ++pivot)
  {
    for (int row = pivot - 1; row >= 0; --row)
    {
      T rowFactor = copy[row][pivot] * (T)-1;
      copy.AddRowMultiple(row, pivot, rowFactor);
      inverse.AddRowMultiple(row, pivot, rowFactor);
    }
  }
  return inverse;
}

template<typename T, unsigned int N>
void Matrix<T, N>::MultiplyRow(int row, T factor)
{
  for (int col = 0; col < N; ++col)
  {
    mValue[row][col] *= factor;
  }
}

template<typename T, unsigned int N>
void Matrix<T, N>::AddRowMultiple(int row, int factorRow, T factor)
{
  for (int col = 0; col < N; ++col)
  {
    mValue[row][col] += mValue[factorRow][col] * factor;
  }
}

template<typename T, unsigned int N>
void Matrix<T, N>::SwapRows(int rowA, int rowB)
{
  for (int col = 0; col < N; ++col)
  {
    T temp = mValue[rowA][col];
    mValue[rowA][col] = mValue[rowB][col];
    mValue[rowB][col] = temp;
  }
}

template<typename T, unsigned int N>
T* Matrix<T, N>::Data()
{
  return (T*)mValue;
}

template<typename T, unsigned int N>
const T* Matrix<T, N>::CData() const
{
  return (const T*)mValue;
}

template<typename T, unsigned int N>
T* Matrix<T, N>::operator[](int row)
{
  return mValue[row];
}

template<typename T, unsigned int N>
Matrix<T, N>& Matrix<T, N>::operator=(const Matrix<T, N>& other)
{
  for (int r = 0; r < N; ++r)
  {
    for (int c = 0; c < N; ++c)
    {
      mValue[r][c] = other.mValue[c][r];
    }
  }
  return *this;
}

template<typename T, unsigned int N>
Matrix<T, N> Matrix<T, N>::operator*(const Matrix<T, N>& other) const
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
  int elementRow, int elementColumn, const Matrix<T, N>& other) const
{
  T result = (T)0;
  for (int i = 0; i < N; ++i)
  {
    result += mValue[elementRow][i] * other.mValue[i][elementColumn];
  }
  return result;
}

template<typename T, unsigned int N>
std::ostream& operator<<(std::ostream& os, Matrix<T, N> mat)
{
  // We find the longest number in each column so that all columns are aligned
  // when printed to console.
  int longestWidths[N];
  for (int c = 0; c < N; ++c)
  {
    longestWidths[c] = 1;
    for (int r = 0; r < N; ++r)
    {
      std::stringstream ss;
      ss << mat[r][c];
      int width = ss.str().size();
      if (width > longestWidths[c])
      {
        longestWidths[c] = width;
      }
    }
  }

  for (int r = 0; r < N; ++r)
  {
    os << "[";
    for (int c = 0; c < N; ++c)
    {
      os << std::setw(longestWidths[c]) << std::right << mat[r][c];
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