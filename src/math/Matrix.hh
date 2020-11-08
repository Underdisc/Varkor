#include <iomanip>
#include <sstream>

#include "Error.h"

namespace Math {

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
  return (T*)mD[row];
}

template<typename T, unsigned int N>
const T* Matrix<T, N>::operator[](int row) const
{
  return (const T*)mD[row];
}

template<typename T, unsigned int N>
Matrix<T, N> operator*(const Matrix<T, N>& a, const Matrix<T, N>& b)
{
  Matrix<T, N> result;
  for (int r = 0; r < N; ++r)
  {
    for (int c = 0; c < N; ++c)
    {
      result[r][c] = CalculateProductElement(a, b, r, c);
    }
  }
  return result;
}

template<typename T, unsigned int N>
Matrix<T, N>& operator*=(Matrix<T, N>& a, const Matrix<T, N>& b)
{
  Matrix<T, N> result = a * b;
  a = result;
  return a;
}

template<typename T, unsigned int N>
std::ostream& operator<<(std::ostream& os, const Matrix<T, N>& matrix)
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
      ss << matrix[r][c];
      int width = (int)ss.str().size();
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
      os << std::setw(longestWidths[c]) << std::right << matrix[r][c];
      if (c < N - 1)
      {
        os << ", ";
      }
    }
    os << "]" << std::endl;
  }
  return os;
}

template<typename T, unsigned int N>
void Zero(Matrix<T, N>* matrix)
{
  for (int r = 0; r < N; ++r)
  {
    for (int c = 0; c < N; ++c)
    {
      (*matrix)[r][c] = (T)0;
    }
  }
}

template<typename T, unsigned int N>
void Identity(Matrix<T, N>* matrix)
{
  Zero(matrix);
  for (int d = 0; d < N; ++d)
  {
    (*matrix)[d][d] = (T)1;
  }
}

template<typename T, unsigned int N>
void Scale(Matrix<T, N>* matrix, const Vector<T, N - 1>& scale)
{
  Zero(matrix);
  for (int i = 0; i < N - 1; ++i)
  {
    (*matrix)[i][i] = scale[i];
  }
  (*matrix)[N - 1][N - 1] = (T)1;
}

template<typename T, unsigned int N>
void Translate(Matrix<T, N>* matrix, const Vector<T, N - 1>& translation)
{
  Identity(matrix);
  for (int i = 0; i < N - 1; ++i)
  {
    (*matrix)[i][N - 1] = translation[i];
  }
}

template<typename T, unsigned int N>
Matrix<T, N> Inverse(const Matrix<T, N>& matrix)
{
  // We find the inverse using Gauss-Jordan elimination. That is, we augment
  // the square matrix by the identity and perform row operations until the new
  // N by 2N matrix is in reduced row echelon form. At this point the inverse
  // matrix is what used to be the identity after the augmentation.

  // These two matrices represent the N by 2N matrix together. We will
  // perform the same row operations on both of these matrices.
  Matrix<T, N> copy = matrix;
  Matrix<T, N> inverse;
  Identity(&inverse);
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
          SwapRows(&copy, row, pivot);
          SwapRows(&inverse, row, pivot);
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
    MultiplyRow(&copy, pivot, rowFactor);
    MultiplyRow(&inverse, pivot, rowFactor);
    for (int row = pivot + 1; row < N; ++row)
    {
      rowFactor = copy[row][pivot] * (T)-1;
      AddRowMultiple(&copy, row, pivot, rowFactor);
      AddRowMultiple(&inverse, row, pivot, rowFactor);
    }
  }

  // We now force all elements that are above pivots to be zeros using row
  // operations.
  for (int pivot = 1; pivot < N; ++pivot)
  {
    for (int row = pivot - 1; row >= 0; --row)
    {
      T rowFactor = copy[row][pivot] * (T)-1;
      AddRowMultiple(&copy, row, pivot, rowFactor);
      AddRowMultiple(&inverse, row, pivot, rowFactor);
    }
  }
  return inverse;
}

template<typename T, unsigned int N>
void MultiplyRow(Matrix<T, N>* matrix, int row, T factor)
{
  for (int col = 0; col < N; ++col)
  {
    (*matrix)[row][col] *= factor;
  }
}

template<typename T, unsigned int N>
void AddRowMultiple(Matrix<T, N>* matrix, int row, int factorRow, T factor)
{
  for (int col = 0; col < N; ++col)
  {
    (*matrix)[row][col] += (*matrix)[factorRow][col] * factor;
  }
}

template<typename T, unsigned int N>
void SwapRows(Matrix<T, N>* matrix, int rowA, int rowB)
{
  for (int col = 0; col < N; ++col)
  {
    T temp = (*matrix)[rowA][col];
    (*matrix)[rowA][col] = (*matrix)[rowB][col];
    (*matrix)[rowB][col] = temp;
  }
}

template<typename T, unsigned int N>
T CalculateProductElement(
  const Matrix<T, N>& a,
  const Matrix<T, N>& b,
  int elementRow,
  int elementColumn)
{
  T result = (T)0;
  for (int i = 0; i < N; ++i)
  {
    result += a[elementRow][i] * b[i][elementColumn];
  }
  return result;
}

} // namespace Math