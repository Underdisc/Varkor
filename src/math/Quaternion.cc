#include <math.h>

#include "math/Quaternion.h"
#include "math/Utility.h"

namespace Math {

Quaternion::Quaternion() {}

Quaternion::Quaternion(float a, float b, float c, float d):
  mVec({a, b, c, d}) {}

Quaternion::Quaternion(const Vec3& from, const Vec3& to) {
  *this = FromTo(from, to);
}

Quaternion::Quaternion(float angle, const Vec3& axis) {
  *this = AngleAxis(angle, axis);
}

Quaternion::Quaternion(
  const Vec3& xAxis, const Vec3& yAxis, const Vec3& zAxis) {
  *this = BasisVectors(xAxis, yAxis, zAxis);
}

void Quaternion::InitIdentity() {
  *this = Identity();
}

void Quaternion::InitAngleAxis(float angle, const Vec3& axis) {
  *this = AngleAxis(angle, axis);
}

void Quaternion::InitFromTo(const Vec3& from, const Vec3& to) {
  *this = FromTo(from, to);
}

void Quaternion::InitBasisVectors(
  const Vec3& xAxis, const Vec3& yAxis, const Vec3& zAxis) {
  *this = BasisVectors(xAxis, yAxis, zAxis);
}

Quaternion Quaternion::Identity() {
  return Quat(1, 0, 0, 0);
}

Quaternion Quaternion::AngleAxis(float angle, Vec3 axis) {
  Quat quat;
  float halfAngle = angle / 2.0f;
  quat.mA = cosf(halfAngle);
  axis = Math::Normalize(axis) * sinf(halfAngle);
  quat.mB = axis[0];
  quat.mC = axis[1];
  quat.mD = axis[2];
  return quat;
}

Quaternion Quaternion::FromTo(const Vec3& from, const Vec3& to) {
  float magSqProduct = Math::MagnitudeSq(from) * Math::MagnitudeSq(to);
  LogAbortIf(magSqProduct == 0.0f, "One of the vectors is a zero vector.");
  Quat quat;
  quat.mA = sqrtf(magSqProduct) + Math::Dot(from, to);
  Vec3 axis;
  if (Near(quat.mA, 0.0f)) {
    axis = Math::PerpendicularTo(from);
  }
  else {
    axis = Math::Cross(from, to);
  }
  quat.mB = axis[0];
  quat.mC = axis[1];
  quat.mD = axis[2];
  quat.Normalize();
  return quat;
}

// This initializes the quaternion using the 3 orthogonal unit vectors
// associated with it. What happens when a proper orthogonal basis is not
// supplied is beyond the scope of this function.
Quaternion Quaternion::BasisVectors(
  const Vec3& xAxis, const Vec3& yAxis, const Vec3& zAxis) {
  // These values are found with the basis vector rotation matrix [X, Y, Z]
  // where X, Y, and Z are the basis vectors for the respective axes. Using
  // that and the quaternion rotation matrix representation, we can calculate
  // the quaternion coefficients.
  Quaternion quat;
  float wSquared = (xAxis[0] + yAxis[1] + zAxis[2] + 1.0f) * 0.25f;
  if (wSquared > 0.0f) {
    quat.mA = sqrtf(wSquared);
    float w4 = quat.mA * 4.0f;
    quat.mB = (yAxis[2] - zAxis[1]) / w4;
    quat.mC = (zAxis[0] - xAxis[2]) / w4;
    quat.mD = (xAxis[1] - yAxis[0]) / w4;
    return quat;
  }
  float xSquared = (xAxis[0] - yAxis[1] - zAxis[2] + 1.0f) * 0.25f;
  if (xSquared > 0.0f) {
    quat.mB = sqrtf(xSquared);
    float x4 = quat.mB * 4.0f;
    quat.mA = (yAxis[2] - zAxis[1]) / x4;
    quat.mC = (xAxis[1] + yAxis[0]) / x4;
    quat.mD = (xAxis[2] + zAxis[0]) / x4;
    return quat;
  }
  float ySquared = (yAxis[1] - xAxis[0] - zAxis[2] + 1.0f) * 0.25f;
  if (ySquared > 0.0f) {
    quat.mC = sqrtf(ySquared);
    float y4 = quat.mC * 4.0f;
    quat.mA = (zAxis[0] - xAxis[2]) / y4;
    quat.mB = (xAxis[1] + yAxis[0]) / y4;
    quat.mD = (yAxis[2] + zAxis[1]) / y4;
    return quat;
  }
  float zSquared = (zAxis[2] - xAxis[0] - yAxis[1] + 1.0f) * 0.25f;
  quat.mD = sqrtf(zSquared);
  float z4 = quat.mD * 4.0f;
  quat.mA = (xAxis[1] - yAxis[0]) / z4;
  quat.mB = (xAxis[2] + zAxis[0]) / z4;
  quat.mC = (yAxis[2] + zAxis[1]) / z4;
  return quat;
}

Quaternion Quaternion::Interpolate(float t) const {
  float halfAngle = acosf(mA);
  halfAngle *= t;
  Vec3 axis = {mB, mC, mD};
  axis = Math::Normalize(axis) * sinf(halfAngle);
  return Quaternion(cosf(halfAngle), axis[0], axis[1], axis[2]);
}

void Quaternion::Normalize() {
  mVec = Math::Normalize(mVec);
}

Quaternion Quaternion::Conjugate() const {
  Quaternion result(mA, -mB, -mC, -mD);
  return result;
}

Quaternion& Quaternion::operator*=(const Quaternion& other) {
  *this = *this * other;
  return *this;
}

const float& Quaternion::operator[](int index) const {
  return mVec[index];
}

float& Quaternion::operator[](int index) {
  return mVec[index];
}

float Quaternion::Magnitude() const {
  return Math::Magnitude(mVec);
}

Vec3 Quaternion::Axis() const {
  Vec3 axis = {mB, mC, mD};
  float magnitude = Math::Magnitude(axis);
  if (magnitude == 0.0f) {
    axis = {0.0f, 0.0f, 0.0f};
    return axis;
  }
  return axis / magnitude;
}

Vec3 Quaternion::EulerAngles() const {
  // The angle order is the rotation around the x, y, and then z axis.
  Vec3 angles;
  float m21 = 2.0f * mC * mD + 2.0f * mB * mA;
  float m22 = 1.0f - 2.0f * mB * mB - 2.0f * mC * mC;
  angles[0] = std::atan2(m21, m22);
  float cPsi = sqrtf(m21 * m21 + m22 * m22);
  float m20 = 2.0f * mB * mD - 2.0f * mC * mA;
  angles[1] = std::atan2(-m20, cPsi);
  float m00 = 1.0f - 2.0f * mC * mC - 2.0f * mD * mD;
  float m10 = 2.0f * mB * mC + 2.0f * mD * mA;
  angles[2] = std::atan2(m10, m00);
  return angles;
}

Vec3 Quaternion::Rotate(const Vec3& vector) const {
  Vec3 axis = {mB, mC, mD};
  Vec3 result = 2.0f * Math::Dot(axis, vector) * axis;
  result += (mA * mA - Math::MagnitudeSq(axis)) * vector;
  result += 2.0f * mA * Math::Cross(axis, vector);
  return result;
}

Vec3 Quaternion::XBasisAxis() const {
  Vec3 axis = {
    1.0f - 2.0f * mC * mC - 2.0f * mD * mD,
    2.0f * mB * mC + 2.0f * mD * mA,
    2.0f * mB * mD - 2.0f * mC * mA};
  return axis;
}

Vec3 Quaternion::YBasisAxis() const {
  Vec3 axis = {
    2.0f * mB * mC - 2.0f * mD * mA,
    1.0f - 2.0f * mB * mB - 2.0f * mD * mD,
    2.0f * mC * mD + 2.0f * mB * mA};
  return axis;
}

Vec3 Quaternion::ZBasisAxis() const {
  Vec3 axis = {
    2.0f * mB * mD + 2.0f * mC * mA,
    2.0f * mC * mD - 2.0f * mB * mA,
    1.0f - 2.0f * mB * mB - 2.0f * mC * mC};
  return axis;
}

Quaternion operator*(const Quaternion& a, const Quaternion& b) {
  Quaternion result;
  result.mA = a.mA * b.mA - a.mB * b.mB - a.mC * b.mC - a.mD * b.mD;
  result.mB = a.mA * b.mB + a.mB * b.mA + a.mC * b.mD - a.mD * b.mC;
  result.mC = a.mA * b.mC - a.mB * b.mD + a.mC * b.mA + a.mD * b.mB;
  result.mD = a.mA * b.mD + a.mB * b.mC - a.mC * b.mB + a.mD * b.mA;
  return result;
}

} // namespace Math
