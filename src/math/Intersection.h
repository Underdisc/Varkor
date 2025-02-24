#ifndef math_Intersection_h
#define math_Intersection_h

#include "math/Box.h"
#include "math/Plane.h"
#include "math/Ray.h"
#include "math/Sphere.h"
#include "math/Triangle.h"

namespace Math {

struct RayPlane {
  bool mIntersecting;
  Vec3 mIntersection;
};
RayPlane Intersection(const Ray& ray, const Plane& plane);

struct SphereSphere {
  bool mIntersecting;
  Vec3 mNormal;
  Vec3 mContactPoint;
  float mPenetration;
};
SphereSphere Intersection(const Sphere& a, const Sphere& b);

bool HasIntersection(const Box& a, const Box& b);

struct SphereTriangle {
  bool mIntersecting;
  Vec3 mSeparation;
};
SphereTriangle Intersection(const Sphere& sphere, const Triangle& triangle);

struct RaySphere {
  int mCount;
  Vec3 mPoints[2];
};
RaySphere Intersection(const Ray& ray, const Sphere& sphere);

} // namespace Math

#endif
