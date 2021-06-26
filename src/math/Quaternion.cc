#include <math.h>

#include "math/Utility.h"

#include "math/Quaternion.h"

namespace Math {

Quaternion::Quaternion() {}

Quaternion::Quaternion(float a, float b, float c, float d): mVec({a, b, c, d})
{}

void Quaternion::Identity()
{
  mVec = {1.0f, 0.0, 0.0f, 0.0f};
}

void Quaternion::AngleAxis(float angle, Vec3 axis)
{
  float halfAngle = angle / 2.0f;
  mA = std::cosf(halfAngle);
  axis = Math::Normalize(axis) * std::sinf(halfAngle);
  mB = axis[0];
  mC = axis[1];
  mD = axis[2];
}

void Quaternion::FromTo(Vec3 from, Vec3 to)
{
  float magSqProduct = Math::MagnitudeSq(from) * Math::MagnitudeSq(to);
  LogAbortIf(magSqProduct == 0.0f, "One of the vectors is a zero vector.");
  mA = std::sqrtf(magSqProduct) + Math::Dot(from, to);
  Vec3 axis;
  if (Near(mA, 0.0f))
  {
    axis = Math::PerpendicularTo(from);
  } else
  {
    axis = Math::Cross(from, to);
  }
  mB = axis[0];
  mC = axis[1];
  mD = axis[2];
  Normalize();
}

void Quaternion::Normalize()
{
  mVec = Math::Normalize(mVec);
}

Quaternion Quaternion::Conjugate() const
{
  Quaternion result(mA, -mB, -mC, -mD);
  return result;
}

Quaternion& Quaternion::operator*=(const Quaternion& other)
{
  *this = *this * other;
  return *this;
}

const float& Quaternion::operator[](int index) const
{
  return mVec[index];
}

float& Quaternion::operator[](int index)
{
  return mVec[index];
}

float Quaternion::Magnitude() const
{
  return Math::Magnitude(mVec);
}

Vec3 Quaternion::Axis() const
{
  Vec3 axis = {mB, mC, mD};
  float magnitude = Math::Magnitude(axis);
  if (magnitude == 0.0f)
  {
    axis = {0.0f, 0.0f, 0.0f};
    return axis;
  }
  return axis / magnitude;
}

Vec3 Quaternion::EulerAngles() const
{
  Vec3 angles;
  float m21 = 2.0f * mC * mD + 2.0f * mB * mA;
  float m22 = 1.0f - 2.0f * mB * mB - 2.0f * mC * mC;
  angles[0] = std::atan2(m21, m22);
  float cPsi = std::sqrtf(m21 * m21 + m22 * m22);
  float m20 = 2.0f * mB * mD - 2.0f * mC * mA;
  angles[1] = std::atan2(-m20, cPsi);
  float m00 = 1.0f - 2.0f * mC * mC - 2.0f * mD * mD;
  float m10 = 2.0f * mB * mC + 2.0f * mD * mA;
  angles[2] = std::atan2(m10, m00);
  return angles;
}

Vec3 Quaternion::Rotate(const Vec3& vector) const
{
  Vec3 axis = {mB, mC, mD};
  Vec3 result = 2.0f * Math::Dot(axis, vector) * axis;
  result += (mA * mA - Math::MagnitudeSq(axis)) * vector;
  result += 2.0f * mA * Math::Cross(axis, vector);
  return result;
}

Quaternion operator*(const Quaternion& a, const Quaternion& b)
{
  Quaternion result;
  result.mA = a.mA * b.mA - a.mB * b.mB - a.mC * b.mC - a.mD * b.mD;
  result.mB = a.mA * b.mB + a.mB * b.mA + a.mC * b.mD - a.mD * b.mC;
  result.mC = a.mA * b.mC - a.mB * b.mD + a.mC * b.mA + a.mD * b.mB;
  result.mD = a.mA * b.mD + a.mB * b.mC - a.mC * b.mB + a.mD * b.mA;
  return result;
}

} // namespace Math
