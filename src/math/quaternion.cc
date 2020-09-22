#include <math.h>

#include "quaternion.h"

namespace Math {

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

Quaternion Quaternion::Conjugate() const
{
  Quaternion result = {mA, -mB, -mC, -mD};
  return result;
}

Quaternion& Quaternion::operator*=(const Quaternion& other)
{
  *this = *this * other;
  return *this;
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
