#include <math.h>

#include "Quaternion.h"

namespace Math {

Quaternion::Quaternion(): mA(1.0f), mB(0.0f), mC(0.0f), mD(0.0f) {}

Quaternion::Quaternion(float angle, Vec3 axis)
{
  AngleAxis(angle, axis);
}

Quaternion::Quaternion(float a, float b, float c, float d):
  mA(a), mB(b), mC(c), mD(d)
{}

void Quaternion::AngleAxis(float angle, Vec3 axis)
{
  float halfAngle = angle / 2.0f;
  mA = std::cosf(halfAngle);
  float axisScaler = std::sinf(halfAngle);
  axis *= axisScaler;
  mB = axis[0];
  mC = axis[1];
  mD = axis[2];
}

void Quaternion::Normalize()
{
  float magnitude = Magnitude();
  LogAbortIf(
    magnitude == 0.0f, "Quaternion with a magnitude of 0 can't be normalized.");
  mA /= magnitude;
  mB /= magnitude;
  mC /= magnitude;
  mD /= magnitude;
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

float Quaternion::Magnitude() const
{
  return std::sqrtf(mA * mA + mB * mB + mC * mC + mD * mD);
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

Quaternion operator*(const Quaternion& a, const Quaternion& b)
{
  Quaternion result;
  result.mA = a.mA * b.mA - a.mB * b.mB - a.mC * b.mC - a.mD * b.mD;
  result.mB = a.mA * b.mB + a.mB * b.mA + a.mC * b.mD - a.mD * b.mC;
  result.mC = a.mA * b.mC - a.mB * b.mD + a.mC * b.mA + a.mD * b.mB;
  result.mD = a.mA * b.mD + a.mB * b.mC - a.mC * b.mB + a.mD * b.mA;
  return result;
}

std::ostream& operator<<(std::ostream& os, const Quaternion& quat)
{
  os << quat.mA << " + i(" << quat.mB << ") + j(" << quat.mC << ") + k("
     << quat.mD << ")";
  return os;
}

} // namespace Math
