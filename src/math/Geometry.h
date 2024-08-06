#ifndef math_Geometry_h
#define math_Geometry_h

#include "math/Box.h"
#include "math/Plane.h"
#include "math/Ray.h"
#include "math/Sphere.h"
#include "math/Triangle.h"

namespace Math {

bool HasIntersection(const Ray& ray, const Plane& plane);
Vec3 Intersection(const Ray& ray, const Plane& plane);

struct SphereSphere
{
  bool mIntersecting;
  // The distance sphere b needs to be moved to be outside of sphere a.
  Vec3 mSeparation;
};
SphereSphere Intersection(const Sphere& a, const Sphere& b);

bool HasIntersection(const Box& a, const Box& b);

struct SphereTriangle
{
  bool mIntersecting;
  Vec3 mSeparation;
};
SphereTriangle Intersection(const Sphere& sphere, const Triangle& triangle);

struct RaySphere
{
  int mCount;
  Vec3 mPoints[2];
};
RaySphere Intersection(const Ray& ray, const Sphere& sphere);

} // namespace Math

#endif
