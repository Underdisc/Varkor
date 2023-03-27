#ifndef math_Geometry_h
#define math_Geometry_h

#include "math/Vector.h"

namespace Math {

struct Ray
{
public:
  Vec3 mStart;

private:
  Vec3 mDirection;

public:
  void InitNormalized(const Vec3& start, const Vec3& normalizedDirection);
  void StartDirection(const Vec3& start, const Vec3& direction);
  void Direction(const Vec3& direction);
  const Vec3& Direction() const;
  Vec3 At(float t) const;
  float ClosestTTo(const Vec3& point) const;
  Vec3 ClosestPointTo(const Vec3& point) const;
  bool HasClosestTo(const Ray& other) const;
  float ClosestTTo(const Ray& other) const;
  Vec3 ClosestPointTo(const Ray& other) const;
};

struct Plane
{
public:
  Vec3 mPoint;

private:
  Vec3 mNormal;

public:
  void InitNormalized(const Vec3& point, const Vec3& normalizedNormal);
  void PointNormal(const Vec3& point, const Vec3& normal);
  void Normal(const Vec3& normal);
  const Vec3& Normal() const;
};

struct Sphere
{
  Vec3 mCenter;
  float mRadius;

  Sphere();
  Sphere(const Vec3& center, float radius);
  void Init(const Vec3& center, float radius);
};

bool HasIntersection(const Ray& ray, const Plane& plane);
Vec3 Intersection(const Ray& ray, const Plane& plane);

struct RaySphere
{
  int mCount;
  Vec3 mPoints[2];
};
RaySphere Intersection(const Ray& ray, const Sphere& sphere);

} // namespace Math

#endif
