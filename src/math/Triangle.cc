#include "math/Triangle.h"

namespace Math {

Vec3 Triangle::BarycentricCoords(const Vec3& p) const {
  const Vec3& a = mPoints[0];
  const Vec3& b = mPoints[1];
  const Vec3& c = mPoints[2];
  Vec3 v0 = b - a;
  Vec3 v1 = c - a;
  Vec3 v2 = p - a;
  float v0dv0 = Dot(v0, v0);
  float v1dv0 = Dot(v1, v0);
  float v1dv1 = Dot(v1, v1);
  float v2dv0 = Dot(v2, v0);
  float v2dv1 = Dot(v2, v1);
  float denomDeterminant = v0dv0 * v1dv1 - v1dv0 * v1dv0;
  LogAbortIf(denomDeterminant == 0, "Collapsed Triangle");
  Vec3 barycentricCoords;
  barycentricCoords[1] = (v2dv0 * v1dv1 - v2dv1 * v1dv0) / denomDeterminant;
  barycentricCoords[2] = (v0dv0 * v2dv1 - v1dv0 * v2dv0) / denomDeterminant;
  barycentricCoords[0] = 1.0f - barycentricCoords[1] - barycentricCoords[2];
  return barycentricCoords;
}

Vec3 Triangle::BarycentricCoordsToPoint(const Vec3& barycentricCoords) const {
  return mPoints[0] * barycentricCoords[0] + mPoints[1] * barycentricCoords[1] +
    mPoints[2] * barycentricCoords[2];
}

Vec3 Triangle::ClosestPointTo(const Vec3& point) const {
  // Find the projection, r, of the point onto the plane formed by the triangle.
  const Vec3& a = mPoints[0];
  const Vec3& b = mPoints[1];
  const Vec3& c = mPoints[2];
  Vec3 ab = b - a, ac = c - a;
  Vec3 normal = Cross(ab, ac);
  float normalMag = Magnitude(normal);
  LogAbortIf(normalMag == 0.0f, "Collapsed Triangle");
  normal /= normalMag;
  Vec3 fromTriPlane = normal * Math::Dot(normal, point - a);
  Vec3 r = point - fromTriPlane;

  // Determine if r lies closest to one the triangle's vertices.
  Vec3 ar = r - a;
  float abdar = Dot(ab, ar), acdar = Dot(ac, ar);
  if (abdar <= 0.0f && acdar <= 0.0f) {
    return a;
  }
  Vec3 ba = a - b, bc = c - b, br = r - b;
  float badbr = Dot(ba, br), bcdbr = Dot(bc, br);
  if (badbr <= 0.0f && bcdbr <= 0.0f) {
    return b;
  }
  Vec3 ca = a - c, cb = b - c, cr = r - c;
  float cadcr = Dot(ca, cr), cbdcr = Dot(cb, cr);
  if (cadcr <= 0.0f && cbdcr <= 0.0f) {
    return c;
  }

  // Determine if r lies closest to one of the triangle's edges.
  Vec3 barycentric = this->BarycentricCoords(r);
  if (barycentric[0] <= 0.0f && bcdbr >= 0.0f && cbdcr >= 0.0f) {
    float t = bcdbr / Dot(bc, bc);
    return b + t * bc;
  }
  if (barycentric[1] <= 0.0f && acdar >= 0.0f && cadcr >= 0.0f) {
    float t = acdar / Dot(ac, ac);
    return a + t * ac;
  }
  if (barycentric[2] <= 0.0f && abdar >= 0.0f && badbr >= 0.0f) {
    float t = abdar / Dot(ab, ab);
    return a + t * ab;
  }

  // r must be within the triangle.
  return BarycentricCoordsToPoint(barycentric);
}

} // namespace Math
