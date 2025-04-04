#include "math/Ray.h"

namespace Math {

Ray Ray::Points(const Vec3& a, const Vec3& b) {
  Ray newRay;
  newRay.mStart = a, newRay.mDirection = Math::Normalize(b - a);
  return newRay;
}

Ray Ray::StartDirection(const Vec3& point, const Vec3& direction) {
  Ray newRay;
  newRay.mStart = point;
  newRay.mDirection = direction;
  return newRay;
}

Ray Ray::StartNormalizeDirection(const Vec3& point, const Vec3& direction) {
  Ray newRay;
  newRay.mStart = point;
  newRay.mDirection = Math::Normalize(direction);
  return newRay;
}

const Vec3& Ray::Direction() const {
  return mDirection;
}

void Ray::Direction(const Vec3& direction) {
  mDirection = direction;
}

void Ray::NormalizeDirection(const Vec3& direction) {
  mDirection = Math::Normalize(direction);
}

Vec3 Ray::At(float t) const {
  return mStart + mDirection * t;
}

float Ray::DistanceSq(const Vec3& point) const {
  return Math::MagnitudeSq(ClosestPointTo(point) - point);
}

float Ray::ClosestTTo(const Vec3& point) const {
  return Math::Dot(mDirection, point - mStart);
}

Vec3 Ray::ClosestPointTo(const Vec3& point) const {
  float t = Math::Dot(mDirection, point - mStart);
  return At(t);
}

bool Ray::HasClosestTo(const Ray& other) const {
  float directionDot = Math::Dot(mDirection, other.mDirection);
  return !Math::Near(directionDot, 1.0f) && !Math::Near(directionDot, -1.0f);
}

float Ray::ClosestTTo(const Ray& other) const {
  Vec3 sd = mStart - other.mStart;
  const Vec3& da = mDirection;
  const Vec3& db = other.mDirection;
  float dadb = Math::Dot(da, db);
  float dasd = Math::Dot(da, sd);
  float dbsd = Math::Dot(db, sd);
  float denom = 1.0f - dadb * dadb;
  LogAbortIf(denom == 0.0f, "The rays are parallel.");
  return (-dasd + dadb * dbsd) / denom;
}

Vec3 Ray::ClosestPointTo(const Ray& other) const {
  return At(ClosestTTo(other));
}

} // namespace Math
