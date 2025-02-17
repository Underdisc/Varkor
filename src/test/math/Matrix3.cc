#include "test/Test.h"

#include "math/Matrix3.h"
#include "math/Print.h"

void Cast()
{
  Mat3 matrix;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      matrix[i][j] = i * 3 + j;
    }
  }
  std::cout << (Mat4)matrix;
}

void Rotate()
{
  Mat3 matrix;
  Quat rotation = {1, 2, 3, 4};
  Math::Rotate(&matrix, rotation);
  std::cout << matrix;
}

int main()
{
  RunTest(Cast);
  RunTest(Rotate);
}