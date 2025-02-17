#ifndef math_Plane_h
#define math_Plane_h

#include "math/Vector.h"

namespace Math {

struct Plane
{
  // A point on the plane.
  Vec3 mPoint;
  // A unit vector perpendicular to the plane.
  Vec3 mNormal;

  Plane();
  // Defines the plane using three points a, b, and c.
  Plane(const Vec3& a, const Vec3& b, const Vec3& c);
  void Init(const Vec3& a, const Vec3& b, const Vec3& c);
  void InitNormalized(const Vec3& point, const Vec3& normalizedNormal);
  void PointNormal(const Vec3& point, const Vec3& normal);
  void Normal(const Vec3& normal);
  const Vec3& Normal() const;
};

} // namespace Math

#endif