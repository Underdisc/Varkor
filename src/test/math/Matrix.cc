#include "math/Matrix.h"
#include "math/Vector.h"

#include <iostream>

Math::Matrix<float, 5> CreateGenericMat5()
{
  Math::Matrix<float, 5> matrix;
  for (int r = 0; r < 5; ++r)
  {
    for (int c = 0; c < 5; ++c)
    {
      matrix[r][c] = r * 5 + c + 1;
    }
  }
  return matrix;
}

void EqualsOperator()
{
  std::cout << "EqualsOperator" << std::endl;
  Math::Matrix<float, 5> matrix = CreateGenericMat5();
  std::cout << "original" << std::endl << matrix;
  Math::Matrix<float, 5> copyMatrix = matrix;
  std::cout << "copy" << std::endl << copyMatrix << std::endl;
}

void Multiply()
{
  std::cout << "Multiply" << std::endl;
  Math::Matrix<float, 5> matrix = CreateGenericMat5();
  Math::Matrix<float, 5> product = matrix * matrix;
  std::cout << "product matrix" << std::endl << product << std::endl;
}

void MultiplyEquals()
{
  std::cout << "MultiplyEquals" << std::endl;
  Math::Matrix<float, 5> matrix = CreateGenericMat5();
  matrix *= matrix;
  std::cout << "product matrix" << std::endl << matrix << std::endl;
}

void Zero()
{
  std::cout << "Zero" << std::endl;
  Math::Matrix<float, 5> matrix;
  Math::Zero(&matrix);
  std::cout << "zero matrix" << std::endl << matrix << std::endl;
}

void Identity()
{
  std::cout << "Identity" << std::endl;
  Math::Matrix<float, 5> matrix;
  Math::Identity(&matrix);
  std::cout << "identity matrix" << std::endl << matrix << std::endl;
}

void Scale()
{
  std::cout << "Scale" << std::endl;
  Math::Matrix<float, 5> matrix;
  Math::Vector<float, 4> scale = {1.0f, 2.0f, 3.0f, 4.0f};
  Math::Scale(&matrix, scale);
  std::cout << "scale matrix" << std::endl << matrix << std::endl;
}

void Translate()
{
  std::cout << "Translate" << std::endl;
  Math::Matrix<float, 5> matrix;
  Math::Vector<float, 4> translate = {1.0f, 2.0f, 3.0f, 4.0f};
  Math::Translate(&matrix, translate);
  std::cout << "translation matrix" << std::endl << matrix << std::endl;
}

void Inverse()
{
  std::cout << "Inverse" << std::endl;
  Math::Matrix<float, 5> matrix;
  Math::Identity(&matrix);
  matrix[0][0] = 0;
  matrix[0][1] = 300;
  matrix[0][3] = 60;
  matrix[1][0] = 22;
  matrix[1][4] = 33;
  matrix[2][0] = 15;
  matrix[2][4] = 17;
  matrix[3][0] = 34;
  matrix[2][1] = 25;
  matrix[4][2] = 8;
  std::cout << "original" << std::endl << matrix;
  Math::Matrix<float, 5> inverse = Math::Inverse(matrix);
  std::cout << "inverse" << std::endl << inverse;

  // This gets very close to the identity, but it's not exactly the identity.
  // The minor error is likely introduced due to floating point.
  Math::Matrix<float, 5> identity = matrix * inverse;
  std::cout << "identity" << std::endl << identity << std::endl;
}

void MultiplyRow()
{
  std::cout << "MultiplyRow" << std::endl;
  Math::Matrix<float, 5> matrix = CreateGenericMat5();
  std::cout << "original" << std::endl << matrix;
  Math::MultiplyRow(&matrix, 1, 4.0f);
  std::cout << "row 1 multiplied by 4" << std::endl << matrix << std::endl;
}

void AddRowMultiple()
{
  std::cout << "AddRowMultiple" << std::endl;
  Math::Matrix<float, 5> matrix = CreateGenericMat5();
  std::cout << "original" << std::endl << matrix;
  Math::AddRowMultiple(&matrix, 0, 1, 2.0f);
  std::cout << "row 1 multiplied by 2 and added to row 0" << std::endl
            << matrix << std::endl;
}

void SwapRows()
{
  std::cout << "SwapRows" << std::endl;
  Math::Matrix<float, 5> matrix = CreateGenericMat5();
  std::cout << "original" << std::endl << matrix;
  Math::SwapRows(&matrix, 1, 2);
  std::cout << "new" << std::endl << matrix << std::endl;
}

void NoInverseZeroRow()
{
  std::cout << "ZeroRow" << std::endl;
  Math::Matrix<float, 5> matrix = CreateGenericMat5();
  // Make a zero row.
  for (int col = 0; col < 5; ++col)
  {
    matrix[1][col] = 0.0f;
  }
  std::cout << matrix;
  Math::Inverse(matrix);
}

void NoInverseZeroColumn()
{
  std::cout << "ZeroColumn" << std::endl;
  Math::Matrix<float, 5> matrix = CreateGenericMat5();
  // Make a zero column.
  for (int row = 0; row < 5; ++row)
  {
    matrix[row][2] = 0.0f;
  }
  std::cout << matrix;
  Math::Inverse(matrix);
}

void NoInverseDependentRow()
{
  std::cout << "DependentRow" << std::endl;
  Math::Matrix<float, 5> matrix = CreateGenericMat5();
  // Create a linearly dependent row.
  for (int col = 0; col < 5; ++col)
  {
    matrix[1][col] = matrix[3][col] * 4.0f;
  }
  std::cout << matrix;
  Math::Inverse(matrix);
}

void NoInverseDependentColumn()
{
  std::cout << "DependentColumn" << std::endl;
  Math::Matrix<float, 5> matrix = CreateGenericMat5();
  // Create a linearly dependent column.
  for (int row = 0; row < 5; ++row)
  {
    matrix[row][2] = matrix[row][0] * 3.0f;
  }
  std::cout << matrix;
  Math::Inverse(matrix);
}

int main(void)
{
  EqualsOperator();
  Multiply();
  MultiplyEquals();
  Zero();
  Identity();
  Scale();
  Translate();
  Inverse();
  MultiplyRow();
  AddRowMultiple();
  SwapRows();

  // These functions test matrices that are not invertible. That is, they have
  // a zero row, a zero column, a linearly dependent row, or a linearly
  // dependent column. Each of these will signal an abort.
  /*
  NoInverseZeroRow();
  NoInverseZeroColumn();
  NoInverseDependentRow();
  NoInverseDependentColumn();
  */
}
