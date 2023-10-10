#ifndef math_Triangle_h
#define math_Triangle_h

#include "math/Vector.h"

namespace Math {

// Given triangle points a, b, and c, this finds the barycentric coordinates of
// p. The barycentric vector's components relate to a, b, and c respectively.
Vec3 BarycentricCoords(
  const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& p);

} // namespace Math

#endif