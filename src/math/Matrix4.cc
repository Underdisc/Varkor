#include <math.h>

#include "Error.h"

#include "Matrix4.h"

namespace Math {

float* Matrix<float, 4>::Data()
{
  return (float*)mD;
}

const float* Matrix<float, 4>::CData() const
{
  return (const float*)mD;
}

float* Matrix<float, 4>::operator[](int row)
{
  return mD[row];
}

const float* Matrix<float, 4>::operator[](int row) const
{
  return mD[row];
}

Matrix<float, 4> operator*(const Matrix<float, 4>& a, const Matrix<float, 4>& b)
{
  Matrix<float, 4> res;
  res[0][0] = a[0][0] * b[0][0] + a[0][1] * b[1][0] + a[0][2] * b[2][0] +
    a[0][3] * b[3][0];
  res[0][1] = a[0][0] * b[0][1] + a[0][1] * b[1][1] + a[0][2] * b[2][1] +
    a[0][3] * b[3][1];
  res[0][2] = a[0][0] * b[0][2] + a[0][1] * b[1][2] + a[0][2] * b[2][2] +
    a[0][3] * b[3][2];
  res[0][3] = a[0][0] * b[0][3] + a[0][1] * b[1][3] + a[0][2] * b[2][3] +
    a[0][3] * b[3][3];

  res[1][0] = a[1][0] * b[0][0] + a[1][1] * b[1][0] + a[1][2] * b[2][0] +
    a[1][3] * b[3][0];
  res[1][1] = a[1][0] * b[0][1] + a[1][1] * b[1][1] + a[1][2] * b[2][1] +
    a[1][3] * b[3][1];
  res[1][2] = a[1][0] * b[0][2] + a[1][1] * b[1][2] + a[1][2] * b[2][2] +
    a[1][3] * b[3][2];
  res[1][3] = a[1][0] * b[0][3] + a[1][1] * b[1][3] + a[1][2] * b[2][3] +
    a[1][3] * b[3][3];

  res[2][0] = a[2][0] * b[0][0] + a[2][1] * b[1][0] + a[2][2] * b[2][0] +
    a[2][3] * b[3][0];
  res[2][1] = a[2][0] * b[0][1] + a[2][1] * b[1][1] + a[2][2] * b[2][1] +
    a[2][3] * b[3][1];
  res[2][2] = a[2][0] * b[0][2] + a[2][1] * b[1][2] + a[2][2] * b[2][2] +
    a[2][3] * b[3][2];
  res[2][3] = a[2][0] * b[0][3] + a[2][1] * b[1][3] + a[2][2] * b[2][3] +
    a[2][3] * b[3][3];

  res[3][0] = a[3][0] * b[0][0] + a[3][1] * b[1][0] + a[3][2] * b[2][0] +
    a[3][3] * b[3][0];
  res[3][1] = a[3][0] * b[0][1] + a[3][1] * b[1][1] + a[3][2] * b[2][1] +
    a[3][3] * b[3][1];
  res[3][2] = a[3][0] * b[0][2] + a[3][1] * b[1][2] + a[3][2] * b[2][2] +
    a[3][3] * b[3][2];
  res[3][3] = a[3][0] * b[0][3] + a[3][1] * b[1][3] + a[3][2] * b[2][3] +
    a[3][3] * b[3][3];
  return res;
}

Matrix<float, 4>& operator*=(Matrix<float, 4>& a, const Matrix<float, 4>& b)
{
  a = a * b;
  return a;
}

void Scale(Matrix<float, 4>* m, const Vec3& s)
{
  // clang-format off
  *m = {s[0], 0.0f, 0.0f, 0.0f,
        0.0f, s[1], 0.0f, 0.0f,
        0.0f, 0.0f, s[2], 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f};
  // clang-format on
}

void Scale(Matrix<float, 4>* m, float uniS)
{
  // clang-format off
  *m = {uniS, 0.0f, 0.0f, 0.0f,
        0.0f, uniS, 0.0f, 0.0f,
        0.0f, 0.0f, uniS, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f};
  // clang-format on
}

void Translate(Matrix<float, 4>* m, const Vec3& t)
{
  // clang-format off
  *m = {1.0f, 0.0f, 0.0f, t[0],
        0.0f, 1.0f, 0.0f, t[1],
        0.0f, 0.0f, 1.0f, t[2],
        0.0f, 0.0f, 0.0f, 1.0f};
  // clang-format on
}

void Rotate(Matrix<float, 4>* m, const Quaternion& rot)
{
  m->mD[0][0] = 1.0f - 2.0f * rot.mC * rot.mC - 2.0f * rot.mD * rot.mD;
  m->mD[0][1] = 2.0f * rot.mB * rot.mC - 2.0f * rot.mD * rot.mA;
  m->mD[0][2] = 2.0f * rot.mB * rot.mD + 2.0f * rot.mC * rot.mA;
  m->mD[0][3] = 0.0f;
  m->mD[1][0] = 2.0f * rot.mB * rot.mC + 2.0f * rot.mD * rot.mA;
  m->mD[1][1] = 1.0f - 2.0f * rot.mB * rot.mB - 2.0f * rot.mD * rot.mD;
  m->mD[1][2] = 2.0f * rot.mC * rot.mD - 2.0f * rot.mB * rot.mA;
  m->mD[1][3] = 0.0f;
  m->mD[2][0] = 2.0f * rot.mB * rot.mD - 2.0f * rot.mC * rot.mA;
  m->mD[2][1] = 2.0f * rot.mC * rot.mD + 2.0f * rot.mB * rot.mA;
  m->mD[2][2] = 1.0f - 2.0f * rot.mB * rot.mB - 2.0f * rot.mC * rot.mC;
  m->mD[2][3] = 0.0f;
  m->mD[3][0] = 0.0f;
  m->mD[3][1] = 0.0f;
  m->mD[3][2] = 0.0f;
  m->mD[3][3] = 1.0f;
}

void Perspective(
  Matrix<float, 4>* m, float fovY, float aspect, float near, float far)
{
  // The fov passed into this function is the angle formed by the top and the
  // bottom of the view fustum, not the left and right.
  float tanHalfFov = std::tanf(fovY / 2.0f);
  float tanHalfFovAspect = tanHalfFov * aspect;
  float nearFarDifference = near - far;

  LogAbortIf(tanHalfFovAspect == 0.0f, "The value of tan(fov/2)*aspect is 0.");
  LogAbortIf(nearFarDifference == 0.0f, "near and far have the same value.");

  Zero(m);
  m->mD[0][0] = 1.0f / tanHalfFovAspect;
  m->mD[1][1] = 1.0f / tanHalfFov;
  m->mD[2][2] = (near + far) / nearFarDifference;
  m->mD[2][3] = 2.0f * near * far / nearFarDifference;
  m->mD[3][2] = -1.0f;
}

void Orthographic(
  Matrix<float, 4>* m, float height, float aspect, float near, float far)
{
  LogAbortIf(height == 0.0f, "height must be a non-zero value.");
  LogAbortIf(near == far, "near and far must be different values.");

  Zero(m);
  float width = height * aspect;
  float zScale = 2.0f / (near - far);
  m->mD[0][0] = 2.0f / width;
  m->mD[1][1] = 2.0f / height;
  m->mD[2][2] = zScale;
  m->mD[2][3] = near * zScale - 1.0f;
  m->mD[3][3] = 1.0f;
}

} // namespace Math
