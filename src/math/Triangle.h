#ifndef math_Triangle_h
#define math_Triangle_h

#include "math/Vector.h"

namespace Math {

struct Triangle
{
  Vec3 mA, mB, mC;
  // Finds the barycentric coordinates of p. The barycentric vector's components
  // relate to a, b, and c respectively.
  Vec3 BarycentricCoords(const Vec3& p) const;
  Vec3 BarycentricCoordsToPoint(const Vec3& barycentricCords) const;
  Vec3 ClosestPointTo(const Vec3& point) const;
};

} // namespace Math

#endif