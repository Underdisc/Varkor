#include <iostream>

#include "math/Constants.h"
#include "math/Matrix4.h"
#include "math/Print.h"
#include "math/Quaternion.h"
#include "test/Test.h"

Mat4 CreateGenericMat4()
{
  Mat4 matrix;
  for (int r = 0; r < 4; ++r) {
    for (int c = 0; c < 4; ++c) {
      matrix[r][c] = (float)r * 4.0f + (float)c + 1.0f;
    }
  }
  return matrix;
}

void Multiply()
{
  Mat4 matrix = CreateGenericMat4();
  Mat4 product = matrix * matrix;
  std::cout << product;
}

void MultiplyEquals()
{
  Mat4 matrix = CreateGenericMat4();
  matrix *= matrix;
  std::cout << matrix;
}

void Scale()
{
  Mat4 matrix;
  Vec3 scale = {1.0f, 2.0f, 3.0f};
  Math::Scale(&matrix, scale);
  std::cout << matrix;
}

void UniformScale()
{
  Mat4 matrix;
  Math::Scale(&matrix, 3.0f);
  std::cout << matrix;
}

void Translate()
{
  Mat4 matrix;
  Vec3 translation = {1.0f, 2.0f, 3.0f};
  Math::Translate(&matrix, translation);
  std::cout << matrix;
}

void Rotate()
{
  Mat4 matrix;
  Math::Quaternion quat = {1.0f, 2.0f, 3.0f, 4.0f};
  Math::Rotate(&matrix, quat);
  std::cout << matrix;
}

void Perspective()
{
  Mat4 matrix;
  Math::Perspective(&matrix, Math::nPi / 2.0f, 1.5f, 0.1f, 100.0f);
  std::cout << matrix;
}

void Orthographic()
{
  auto useInputs = [](float height, float aspect, float near, float far)
  {
    Mat4 matrix;
    Math::Orthographic(&matrix, height, aspect, near, far);
    std::cout << matrix;
  };
  useInputs(5.0f, 2.0f, 0.0f, 100.0f);
  useInputs(30.0f, 1.5f, 1.0f, 200.0f);
  useInputs(15.0f, 10.0f, 0.5f, 1000.0f);
}

int main(void)
{
  RunTest(Multiply);
  RunTest(MultiplyEquals);
  RunTest(Scale);
  RunTest(UniformScale);
  RunTest(Translate);
  RunTest(Rotate);
  RunTest(Perspective);
  RunTest(Orthographic);
}
