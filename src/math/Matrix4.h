#ifndef math_Matrix4_h
#define math_Matrix4_h

#include "math/Matrix.h"
#include "math/Quaternion.h"
#include "math/Vector.h"

typedef Math::Matrix<float, 4> Mat4;

namespace Math {

template<>
struct Matrix<float, 4> {
  float mD[4][4];

  float* Data();
  const float* CData() const;
  float* operator[](int row);
  const float* operator[](int row) const;
};

Matrix<float, 4> operator*(
  const Matrix<float, 4>& a, const Matrix<float, 4>& b);
Matrix<float, 4>& operator*=(Matrix<float, 4>& a, const Matrix<float, 4>& b);

void Scale(Matrix<float, 4>* matrix, const Vec3& scale);
void Scale(Matrix<float, 4>* matrix, float uniformScale);
void Translate(Matrix<float, 4>* matrix, const Vec3& translation);
void Rotate(Matrix<float, 4>* matrix, const Quaternion& rotation);
void Perspective(
  Matrix<float, 4>* m, float fovY, float aspect, float near, float far);
void Orthographic(
  Matrix<float, 4>* m, float height, float aspect, float near, float far);

} // namespace Math

#endif
