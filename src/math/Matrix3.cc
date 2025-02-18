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

void Rotate(Mat3* m, const Quaternion& r) {
  m->mD[0][0] = 1.0f - 2.0f * r.mC * r.mC - 2.0f * r.mD * r.mD;
  m->mD[0][1] = 2.0f * r.mB * r.mC - 2.0f * r.mD * r.mA;
  m->mD[0][2] = 2.0f * r.mB * r.mD + 2.0f * r.mC * r.mA;

  m->mD[1][0] = 2.0f * r.mB * r.mC + 2.0f * r.mD * r.mA;
  m->mD[1][1] = 1.0f - 2.0f * r.mB * r.mB - 2.0f * r.mD * r.mD;
  m->mD[1][2] = 2.0f * r.mC * r.mD - 2.0f * r.mB * r.mA;

  m->mD[2][0] = 2.0f * r.mB * r.mD - 2.0f * r.mC * r.mA;
  m->mD[2][1] = 2.0f * r.mC * r.mD + 2.0f * r.mB * r.mA;
  m->mD[2][2] = 1.0f - 2.0f * r.mB * r.mB - 2.0f * r.mC * r.mC;
}

} // namespace Math
