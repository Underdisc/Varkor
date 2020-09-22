#include "../math/matrix_4.h"
#include "../math/quaternion.h"

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

void Multiply()
{
  std::cout << "Multiply" << std::endl;
  Mat4 matrix = CreateGenericMat4();
  Mat4 product = matrix * matrix;
  std::cout << "product" << std::endl << product << std::endl;
}

void MultiplyEquals()
{
  std::cout << "MultiplyEquals" << std::endl;
  Mat4 matrix = CreateGenericMat4();
  matrix *= matrix;
  std::cout << "product" << std::endl << matrix << std::endl;
}

void Scale()
{
  std::cout << "Scale" << std::endl;
  Mat4 matrix;
  Vec3 scale = {1.0f, 2.0f, 3.0f};
  Math::Scale(&matrix, scale);
  std::cout << "matrix" << std::endl << matrix << std::endl;
}

void UniformScale()
{
  std::cout << "UniformScale" << std::endl;
  Mat4 matrix;
  Math::Scale(&matrix, 3.0f);
  std::cout << "matrix" << std::endl << matrix << std::endl;
}

void Translate()
{
  std::cout << "Translate" << std::endl;
  Mat4 matrix;
  Vec3 translation = {1.0f, 2.0f, 3.0f};
  Math::Translate(&matrix, translation);
  std::cout << "matrix" << std::endl << matrix << std::endl;
}

void Rotate()
{
  std::cout << "Rotate" << std::endl;
  Mat4 matrix;
  Quat quat = {1.0f, 2.0f, 3.0f, 4.0f};
  Math::Rotate(&matrix, quat);
  std::cout << "matrix" << std::endl << matrix << std::endl;
}

int main(void)
{
  Multiply();
  MultiplyEquals();
  Scale();
  UniformScale();
  Translate();
  Rotate();
}
