#include "math/Box.h"
#include "math/Matrix3.h"

namespace Math {

Box::Box() {}

Box::Box(const Vec3& center, const Vec3& scale, const Quat& rotation)
{
  Init(center, scale, rotation);
}

void Box::Init(const Vec3& center, const Vec3& scale, const Quat& rotation)
{
  mCenter = center;
  mScale = scale;
  mRotation = rotation;
}

Vec3 Box::Support(const Vec3& direction) const
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

bool HasIntersection(const Box& a, const Box& b)
{
  // Returns true when there is an overlap on the axis.
  auto OverlapOnAxis = [&](const Vec3& axis) -> bool
  {
    Vec3 aSupport = a.Support(axis);
    Vec3 raVec = aSupport - a.mCenter;
    float ra = Math::Abs(Math::Dot(raVec, axis));

    Vec3 bSupport = b.Support(axis);
    Vec3 rbVec = bSupport - b.mCenter;
    float rb = Math::Abs(Math::Dot(rbVec, axis));

    Vec3 tVec = b.mCenter - a.mCenter;
    float t = Math::Abs(Math::Dot(tVec, axis));

    return t <= ra + rb;
  };

  Mat3 aOrientation, bOrientation;
  Math::Rotate(&aOrientation, a.mRotation);
  Math::Rotate(&bOrientation, b.mRotation);

  // Test axes perpendicular to the planes of box boxes.
  for (int i = 0; i < 3; ++i) {
    if (!OverlapOnAxis(Math::GetBasisVector(aOrientation, i))) {
      return false;
    }
    if (!OverlapOnAxis(Math::GetBasisVector(bOrientation, i))) {
      return false;
    }
  }

  // Take the cross product of each edge from one box with each edge from the
  // other box (cartesian product of edges) and use those as test axes.
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      Vec3 aEdge = Math::GetBasisVector(aOrientation, i);
      Vec3 bEdge = Math::GetBasisVector(bOrientation, j);
      Vec3 testAxis = Math::Cross(aEdge, bEdge);
      if (Math::Near(testAxis, {0, 0, 0})) {
        continue;
      }
      testAxis = Math::Normalize(testAxis);
      if (!OverlapOnAxis(testAxis)) {
        return false;
      }
    }
  }

  return true;
}

} // namespace Math