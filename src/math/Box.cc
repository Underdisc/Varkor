#include "math/Box.h"
#include "math/Matrix3.h"

namespace Math {

void Box::Init(const Vec3& center, const Vec3& scale, const Quat& rotation)
{
  mCenter = center;
  mScale = scale;
  mRotation = rotation;
}

Vec3 Box::Support(const Vec3& direction)
{
  // Orient the direction vector relative to the box's orientation.
  Mat3 orientation;
  Rotate(&orientation, mRotation);
  Mat3 inverseOrientation = Math::Transpose(orientation);
  Vec3 localDirection = inverseOrientation * direction;

  // Move away from the box center using the half extents of the box.
  Vec3 support = mCenter;
  for (int i = 0; i < 3; ++i) {
    float sign = Sign(localDirection[i]);
    Vec3 basisVector = GetBasisVector(orientation, i);
    support += (sign * mScale[i] / 2) * basisVector;
  }
  return support;
}

} // namespace Math