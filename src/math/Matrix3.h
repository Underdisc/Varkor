#ifndef math_Matrix3_h
#define math_Matrix3_h

#include "math/Matrix.h"
#include "math/Matrix4.h"
#include "math/Quaternion.h"

typedef Math::Matrix<float, 3> Mat3;

namespace Math {

template<>
struct Matrix<float, 3>
{
  float mD[3][3];

  float* Data();
  const float* CData() const;
  float* operator[](int row);
  const float* operator[](int row) const;
  operator Mat4() const;
};

void Rotate(Mat3* matrix, const Quaternion& rotation);

} // namespace Math

#endif