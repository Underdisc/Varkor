#include "math/Ray.h"

namespace Math {

void Ray::InitNormalized(const Vec3& start, const Vec3& normalizedDirection) {
  mStart = start;
  mDirection = normalizedDirection;
}

void Ray::StartDirection(const Vec3& start, const Vec3& direction) {
  mStart = start;
  Direction(direction);
}

void Ray::Direction(const Vec3& direction) {
  mDirection = Normalize(direction);
}

const Vec3& Ray::Direction() const {
  return mDirection;
}

Vec3 Ray::At(float t) const {
  return mStart + mDirection * t;
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
  const Vec3& da = Direction();
  const Vec3& db = other.Direction();
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
