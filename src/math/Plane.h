#ifndef math_Plane_h
#define math_Plane_h

#include "math/Vector.h"

namespace Math {

struct Plane {
public:
  // A point on the plane.
  Vec3 mPoint;

private:
  // A unit vector perpendicular to the plane.
  Vec3 mNormal;

public:
  // Defines the plane using three points a, b, and c.
  static Plane Points(const Vec3& a, const Vec3& b, const Vec3& c);
  static Plane PointNormal(const Vec3& point, const Vec3& normal);
  static Plane PointNormalizeNormal(const Vec3& point, const Vec3& normal);

  bool HalfSpaceContains(const Vec3& point) const;
  void Normal(const Vec3& normal);
  const Vec3& Normal() const;
};

} // namespace Math

#endif
