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

  // Handle no intersection and the degenerate case.
  if (sqrDist > sqrMaxDist) {
    info.mIntersecting = false;
    return info;
  }
  float dist = Math::Magnitude(distVec);
  if (Near(dist, 0)) {
    info.mIntersecting = false;
    return info;
  }

  // Generate the contact information.
  info.mIntersecting = true;
  info.mNormal = distVec / dist;
  info.mPenetration = dist - radiusSum;
  Vec3 aEdge = a.mCenter + a.mRadius * info.mNormal;
  info.mContactPoint = aEdge + info.mNormal * (info.mPenetration / 2.0f);
  return info;
}

SphereCapsule Intersection(const Sphere& sphere, const Capsule& capsule) {
  // Find the vector describing direction between capsule centers.
  SphereCapsule info;
  const Vec3& cC1 = capsule.mCenters[0];
  const Vec3& cC2 = capsule.mCenters[1];
  Vec3 cDist = cC2 - cC1;
  float cL = Math::Magnitude(cDist);
  Vec3 cDir = cDist / cL;

  // Find closest point to the sphere's center on the line formed using the
  // capsule's centers.
  const Vec3& sC = sphere.mCenter;
  float cT = Math::Dot(sC - cC1, cDir);
  cT = Math::Max(0.0f, Math::Min(cT, cL));
  Vec3 cN = cC1 + cDir * cT;

  // Determine whether a collision occurred and generate contact information.
  const float& sR = sphere.mRadius;
  float collisionStart = sR + capsule.mRadius;
  float scDist = Math::Magnitude(sC - cN);
  if (scDist > collisionStart || Math::Near(scDist, 0)) {
    info.mIntersecting = false;
    return info;
  }
  info.mIntersecting = true;
  info.mPenetration = scDist - collisionStart;
  info.mNormal = Math::Normalize(cN - sC);
  info.mContactPoint = sC + info.mNormal * (sR + info.mPenetration / 2.0f);
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
