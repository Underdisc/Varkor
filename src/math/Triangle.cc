#include "math/Triangle.h"

namespace Math {

Vec3 Triangle::BarycentricCoords(const Vec3& p) const
{
  Vec3 v0 = mB - mA;
  Vec3 v1 = mC - mA;
  Vec3 v2 = p - mA;
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

Vec3 Triangle::BarycentricCoordsToPoint(const Vec3& barycentricCoords) const
{
  return mA * barycentricCoords[0] + mB * barycentricCoords[1] +
    mC * barycentricCoords[2];
}

Vec3 Triangle::ClosestPointTo(const Vec3& point) const
{
  // Find the projection, r, of the point onto the plane formed by the triangle.
  Vec3 ab = mB - mA, ac = mC - mA;
  Vec3 normal = Cross(ab, ac);
  float normalMag = Magnitude(normal);
  LogAbortIf(normalMag == 0.0f, "Collapsed Triangle");
  normal /= normalMag;
  Vec3 fromTriPlane = normal * Math::Dot(normal, point - mA);
  Vec3 r = point - fromTriPlane;

  // Determine if r lies closest to one the triangle's vertices.
  Vec3 ar = r - mA;
  float abdar = Dot(ab, ar), acdar = Dot(ac, ar);
  if (abdar <= 0.0f && acdar <= 0.0f) {
    return mA;
  }
  Vec3 ba = mA - mB, bc = mC - mB, br = r - mB;
  float badbr = Dot(ba, br), bcdbr = Dot(bc, br);
  if (badbr <= 0.0f && bcdbr <= 0.0f) {
    return mB;
  }
  Vec3 ca = mA - mC, cb = mB - mC, cr = r - mC;
  float cadcr = Dot(ca, cr), cbdcr = Dot(cb, cr);
  if (cadcr <= 0.0f && cbdcr <= 0.0f) {
    return mC;
  }

  // Determine if r lies closest to one of the triangle's edges.
  Vec3 barycentric = BarycentricCoords(r);
  if (barycentric[0] <= 0.0f && bcdbr >= 0.0f && cbdcr >= 0.0f) {
    float t = bcdbr / Dot(bc, bc);
    return mB + t * bc;
  }
  if (barycentric[1] <= 0.0f && acdar >= 0.0f && cadcr >= 0.0f) {
    float t = acdar / Dot(ac, ac);
    return mA + t * ac;
  }
  if (barycentric[2] <= 0.0f && abdar >= 0.0f && badbr >= 0.0f) {
    float t = abdar / Dot(ab, ab);
    return mA + t * ab;
  }

  // r must be within the triangle.
  return BarycentricCoordsToPoint(barycentric);
}

} // namespace Math