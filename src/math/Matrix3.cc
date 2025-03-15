#include "math/Matrix3.h"

namespace Math {

float* Mat3::Data() {
  return (float*)mD;
}

const float* Mat3::CData() const {
  return (const float*)mD;
}

float* Mat3::operator[](int row) {
  return mD[row];
}

const float* Mat3::operator[](int row) const {
  return mD[row];
}

Mat3::operator Mat4() const {
  Mat4 m;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      m[i][j] = mD[i][j];
    }
    m[i][3] = 0;
    m[3][i] = 0;
  }
  m[3][3] = 1;
  return m;
}

void Rotate(Mat3* m, const Quaternion& q) {
  Vec3 xAxis = q.XBasisAxis();
  Vec3 yAxis = q.YBasisAxis();
  Vec3 zAxis = q.ZBasisAxis();
  for (int i = 0; i < 3; ++i) {
    m->mD[i][0] = xAxis[i];
    m->mD[i][1] = yAxis[i];
    m->mD[i][2] = zAxis[i];
  }
}

} // namespace Math
