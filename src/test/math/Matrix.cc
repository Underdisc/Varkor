#include "math/Matrix.h"
#include "math/Constants.h"
#include "math/Matrix4.h"
#include "math/Print.h"
#include "math/Vector.h"

#include <iostream>

Math::Matrix<float, 5> CreateGenericMat5()
{
  Math::Matrix<float, 5> matrix;
  for (int r = 0; r < 5; ++r) {
    for (int c = 0; c < 5; ++c) {
      matrix[r][c] = (float)r * 5.0f + (float)c + 1.0f;
    }
  }
  return matrix;
}

void EqualsOperator()
{
  std::cout << "<= EqualsOperator =>" << std::endl;
  Math::Matrix<float, 5> matrix = CreateGenericMat5();
  std::cout << "original" << std::endl << matrix;
  Math::Matrix<float, 5> copyMatrix = matrix;
  std::cout << "copy" << std::endl << copyMatrix << std::endl;
}

void Multiply()
{
  std::cout << "<= Multiply =>" << std::endl;
  Math::Matrix<float, 5> matrix = CreateGenericMat5();
  Math::Matrix<float, 5> product = matrix * matrix;
  std::cout << "product matrix" << std::endl << product << std::endl;
}

void MultiplyEquals()
{
  std::cout << "<= MultiplyEquals =>" << std::endl;
  Math::Matrix<float, 5> matrix = CreateGenericMat5();
  matrix *= matrix;
  std::cout << "product matrix" << std::endl << matrix << std::endl;
}

void MultiplyVector()
{
  std::cout << "<= MultiplyVector =>" << std::endl;
  Math::Matrix<float, 5> matrix = CreateGenericMat5();
  Math::Vector<float, 5> vector = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
  Math::Vector<float, 5> result = matrix * vector;
  std::cout << result << std::endl << std::endl;
}

void Zero()
{
  std::cout << "<= Zero =>" << std::endl;
  Math::Matrix<float, 5> matrix;
  Math::Zero(&matrix);
  std::cout << "zero matrix" << std::endl << matrix << std::endl;
}

void Identity()
{
  std::cout << "<= Identity =>" << std::endl;
  Math::Matrix<float, 5> matrix;
  Math::Identity(&matrix);
  std::cout << "identity matrix" << std::endl << matrix << std::endl;
}

void Transpose()
{
  std::cout << "<= Transpose =>" << std::endl;
  Math::Matrix<float, 5> matrix;
  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 5; ++j) {
      matrix[i][j] = (float)(i * 5 + j);
    }
  }
  Math::Matrix<float, 5> transpose = Math::Transpose(matrix);
  std::cout << transpose << '\n';
}

template<unsigned int N, unsigned int M>
void ResizePrint()
{
  Math::Matrix<float, M> original;
  unsigned int min = Math::Min(N, M);
  for (int i = 0; i < min; ++i) {
    for (int j = 0; j < min; ++j) {
      original[i][j] = i + j;
    }
  }
  Math::Matrix<float, N> resized;
  Math::Resize(&resized, original);
  std::cout << resized << '\n';
}

void Resize()
{
  std::cout << "<= Resize =>" << std::endl;
  ResizePrint<2, 4>();
  ResizePrint<4, 2>();
  ResizePrint<5, 5>();
}

void Scale()
{
  std::cout << "<= Scale =>" << std::endl;
  Math::Matrix<float, 5> matrix;
  Math::Vector<float, 4> scale = {1.0f, 2.0f, 3.0f, 4.0f};
  Math::Scale(&matrix, scale);
  std::cout << "scale matrix" << std::endl << matrix << std::endl;
}

void Translate()
{
  std::cout << "<= Translate =>" << std::endl;
  Math::Matrix<float, 5> matrix;
  Math::Vector<float, 4> translate = {1.0f, 2.0f, 3.0f, 4.0f};
  Math::Translate(&matrix, translate);
  std::cout << "translation matrix" << std::endl << matrix << std::endl;
}

void ApplyToPointVector()
{
  std::cout << "<= ApplyToPointVector =>" << std::endl;
  Vec3 value = {1.0f, 1.0f, 1.0f};
  Math::Matrix<float, 4> scale;
  Math::Matrix<float, 4> translate;
  Math::Scale(&scale, {1.0f, 2.0f, 3.0f});
  Math::Translate(&translate, {1.0f, 1.0f, 1.0f});
  Math::Matrix<float, 4> matrix = translate * scale;
  std::cout << Math::ApplyToPoint(matrix, value) << std::endl;
  std::cout << Math::ApplyToVector(matrix, value) << std::endl << std::endl;
}

void Inverse()
{
  std::cout << "<= Inverse =>" << std::endl;
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

void HomogeneousOrthogonalInverse()
{
  std::cout << "<= HomogeneousOrthogonalInverse =>" << std::endl;
  Quat rotation;
  rotation.AngleAxis(Math::nPi / 4.0f, {1.0f, 1.0f, 1.0f});
  Mat4 translate, rotate;
  Math::Translate(&translate, {1.0f, 2.0f, 3.0f});
  Math::Rotate(&rotate, rotation);
  Mat4 matrix = translate * rotate;
  Mat4 inverseMatrix = Math::HomogeneousOrthogonalInverse(matrix);
  std::cout << inverseMatrix * matrix << std::endl;
}

void MultiplyRow()
{
  std::cout << "<= MultiplyRow =>" << std::endl;
  Math::Matrix<float, 5> matrix = CreateGenericMat5();
  std::cout << "original" << std::endl << matrix;
  Math::MultiplyRow(&matrix, 1, 4.0f);
  std::cout << "row 1 multiplied by 4" << std::endl << matrix << std::endl;
}

void AddRowMultiple()
{
  std::cout << "<= AddRowMultiple =>" << std::endl;
  Math::Matrix<float, 5> matrix = CreateGenericMat5();
  std::cout << "original" << std::endl << matrix;
  Math::AddRowMultiple(&matrix, 0, 1, 2.0f);
  std::cout << "row 1 multiplied by 2 and added to row 0" << std::endl
            << matrix << std::endl;
}

void SwapRows()
{
  std::cout << "<= SwapRows =>" << std::endl;
  Math::Matrix<float, 5> matrix = CreateGenericMat5();
  std::cout << "original" << std::endl << matrix;
  Math::SwapRows(&matrix, 1, 2);
  std::cout << "new" << std::endl << matrix << std::endl;
}

void NoInverseZeroRow()
{
  std::cout << "<= ZeroRow =>" << std::endl;
  Math::Matrix<float, 5> matrix = CreateGenericMat5();
  // Make a zero row.
  for (int col = 0; col < 5; ++col) {
    matrix[1][col] = 0.0f;
  }
  std::cout << matrix;
  Math::Inverse(matrix);
}

void NoInverseZeroColumn()
{
  std::cout << "<= ZeroColumn =>" << std::endl;
  Math::Matrix<float, 5> matrix = CreateGenericMat5();
  // Make a zero column.
  for (int row = 0; row < 5; ++row) {
    matrix[row][2] = 0.0f;
  }
  std::cout << matrix;
  Math::Inverse(matrix);
}

void NoInverseDependentRow()
{
  std::cout << "<= DependentRow =>" << std::endl;
  Math::Matrix<float, 5> matrix = CreateGenericMat5();
  // Create a linearly dependent row.
  for (int col = 0; col < 5; ++col) {
    matrix[1][col] = matrix[3][col] * 4.0f;
  }
  std::cout << matrix;
  Math::Inverse(matrix);
}

void NoInverseDependentColumn()
{
  std::cout << "<= DependentColumn =>" << std::endl;
  Math::Matrix<float, 5> matrix = CreateGenericMat5();
  // Create a linearly dependent column.
  for (int row = 0; row < 5; ++row) {
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
  MultiplyVector();
  Zero();
  Identity();
  Transpose();
  Resize();
  Scale();
  Translate();
  ApplyToPointVector();
  Inverse();
  HomogeneousOrthogonalInverse();
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
