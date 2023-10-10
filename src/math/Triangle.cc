#include "math/Triangle.h"

namespace Math {

Vec3 BarycentricCoords(
  const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& p)
{
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

} // namespace Math