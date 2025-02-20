#include "math/Intersection.h"
#include "math/Matrix3.h"
#include "math/Utility.h"

namespace Math {

RayPlane Intersection(const Ray& ray, const Plane& plane) {
  RayPlane info;
  float nd = Math::Dot(plane.Normal(), ray.Direction());
  if (nd == 0.0f) {
    info.mIntersecting = false;
    return info;
  }
  info.mIntersecting = true;
  float np = Math::Dot(plane.Normal(), plane.mPoint);
  float ns = Math::Dot(plane.Normal(), ray.mStart);
  float t = (np - ns) / nd;
  info.mIntersection = ray.At(t);
  return info;
}

SphereSphere Intersection(const Sphere& a, const Sphere& b) {
  // Determine whether there is an intersection.
  SphereSphere info;
  Vec3 distVec = b.mCenter - a.mCenter;
  float sqrDist = Math::Dot(distVec, distVec);
  float radiusSum = a.mRadius + b.mRadius;
  float sqrMaxDist = radiusSum * radiusSum;
  if (sqrDist > sqrMaxDist) {
    info.mIntersecting = false;
    return info;
  }
  info.mIntersecting = true;

  // Find the separation vector when there is an intersection.
  float dist = Math::Magnitude(distVec);
  if (Near(dist, 0)) {
    info.mSeparation = {radiusSum, 0, 0};
    return info;
  }
  info.mSeparation = (radiusSum - dist) * (distVec / dist);
  return info;
}

RaySphere Intersection(const Ray& ray, const Sphere& sphere) {
  Vec3 relativeStart = ray.mStart - sphere.mCenter;
  float a = Dot(ray.Direction(), ray.Direction());
  float b = 2 * Dot(relativeStart, ray.Direction());
  float c = Dot(relativeStart, relativeStart) - sphere.mRadius * sphere.mRadius;
  float disc = b * b - 4 * a * c;

  RaySphere intersection;
  if (disc < 0) {
    intersection.mCount = 0;
    return intersection;
  }
  if (Near(disc, 0)) {
    intersection.mCount = 1;
    float t = -b / (2 * a);
    intersection.mPoints[0] = ray.At(t);
    return intersection;
  }
  intersection.mCount = 2;
  float t = sqrt(disc);
  intersection.mPoints[0] = ray.At((-b - t) / (2 * a));
  intersection.mPoints[1] = ray.At((-b + t) / (2 * a));
  return intersection;
}

bool HasIntersection(const Box& a, const Box& b) {
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

SphereTriangle Intersection(const Sphere& sphere, const Triangle& tri) {
  Vec3 closestTriPoint = tri.ClosestPointTo(sphere.mCenter);
  Vec3 distVec = sphere.mCenter - closestTriPoint;
  float distSq = MagnitudeSq(distVec);
  float radiusSq = sphere.mRadius * sphere.mRadius;
  SphereTriangle intersection;
  intersection.mIntersecting = distSq <= radiusSq;
  if (!intersection.mIntersecting) {
    return intersection;
  }

  // Find the separation vector since there's an intersection.
  float dist = std::sqrtf(distSq);
  if (Near(dist, 0)) {
    const Vec3& a = tri.mPoints[0];
    const Vec3& b = tri.mPoints[1];
    const Vec3& c = tri.mPoints[2];
    intersection.mSeparation = Cross(b - a, c - a);
    intersection.mSeparation = Normalize(intersection.mSeparation);
    intersection.mSeparation *= sphere.mRadius;
    return intersection;
  }
  float separationDist = sphere.mRadius - dist;
  Vec3 separationDir = distVec / dist;
  intersection.mSeparation = separationDir * separationDist;
  return intersection;
}

} // namespace Math
