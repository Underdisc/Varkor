#define ERROR_NO_ABORT

#include "../math/matrix.hh"
#include "../math/vector.hh"

#include <iostream>

Mat4 CreateGenericMat4()
{
  Mat4 matrix;
  for (int r = 0; r < 4; ++r)
  {
    for (int c = 0; c < 4; ++c)
    {
      matrix[r][c] = r * 4 + c + 1;
    }
  }
  return matrix;
}

void MultiplyRow()
{
  Mat4 matrix = CreateGenericMat4();
  std::cout << "original" << std::endl << matrix;
  matrix.MultiplyRow(1, 4);
  std::cout << "row 1 multiplied by 4" << std::endl << matrix;
}

void AddRowMultiple()
{
  Mat4 matrix = CreateGenericMat4();
  std::cout << "original" << std::endl << matrix;
  matrix.AddRowMultiple(0, 1, 2);
  std::cout << "row 1 multiplied by 2 and added to row 0" << std::endl
            << matrix;
}

void SwapRows()
{
  Mat4 matrix = CreateGenericMat4();
  std::cout << "original" << std::endl << matrix;
  matrix.SwapRows(1, 2);
  std::cout << "new" << std::endl << matrix;
}

void EqualsOperator()
{
  Mat4 matrix = CreateGenericMat4();
  std::cout << "original" << std::endl << matrix;
  Mat4 copy_matrix = matrix;
  std::cout << "copy" << std::endl << copy_matrix;
}

void NoInverseZeroRow()
{
  Mat4 matrix = CreateGenericMat4();
  // Make a zero row.
  for (int col = 0; col < 4; ++col)
  {
    matrix[1][col] = 0.0f;
  }
  std::cout << "original" << std::endl << matrix;
  matrix.Inverse();
}

void NoInverseZeroColumn()
{
  Mat4 matrix = CreateGenericMat4();
  // Make a zero column.
  for (int row = 0; row < 4; ++row)
  {
    matrix[row][2] = 0.0f;
  }
  std::cout << "original" << std::endl << matrix;
  matrix.Inverse();
}

void NoInverseDependentRow()
{
  Mat4 matrix = CreateGenericMat4();
  // Create a linearly dependent row.
  for (int col = 0; col < 4; ++col)
  {
    matrix[1][col] = matrix[3][col] * 4.0f;
  }
  std::cout << "original" << std::endl << matrix;
  matrix.Inverse();
}

void NoInverseDependentColumn()
{
  Mat4 matrix = CreateGenericMat4();
  // Create a linearly dependent column.
  for (int row = 0; row < 4; ++row)
  {
    matrix[row][2] = matrix[row][0] * 3.0f;
  }
  std::cout << "original" << std::endl << matrix;
  matrix.Inverse();
}

void Inverse()
{
  // These functions test matrices that are not invertible. That is, they have
  // a zero row, a zero column, a linearly dependent row, or a linearly
  // dependent column.
  // NoInverseZeroRow();
  // NoInverseZeroColumn();
  // NoInverseDependentRow();
  // NoInverseDependentColumn();

  Mat4 matrix;
  matrix.Identity();
  matrix[0][0] = 0;
  matrix[0][1] = 300;
  matrix[0][3] = 60;
  matrix[1][0] = 22;
  matrix[2][0] = 15;
  matrix[3][0] = 34;
  matrix[2][1] = 25;
  std::cout << "original" << std::endl << matrix;
  Mat4 inverse = matrix.Inverse();
  std::cout << "inverse" << std::endl << inverse;

  // This gets very close to the identity, but it's not exactly the identity.
  // The minor error is likely introduced due to floating point.
  Mat4 identity = matrix * inverse;
  std::cout << "identity" << std::endl << identity;
}

int main(void)
{
  // MultiplyRow();
  // AddRowMultiple();
  // SwapRows();
  // EqualsOperator();
  // Inverse();
}
