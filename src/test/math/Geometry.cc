#include <iostream>

#include "math/Geometry.h"
#include "test/Test.h"

std::ostream& operator<<(std::ostream& os, const Math::Ray& ray)
{
  os << "{" << ray.mStart << ", " << ray.Direction() << "}";
  return os;
}

std::ostream& operator<<(std::ostream& os, const Math::RaySphere& intersection)
{
  os << '{';
  if (intersection.mCount > 0) {
    os << intersection.mPoints[0];
  }
  for (int i = 1; i < intersection.mCount; ++i) {
    os << ", " << intersection.mPoints[i];
  }
  os << '}';
  return os;
}

void At()
{
  Math::Ray a, b;
  a.StartDirection({0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
  std::cout << a.At(0.0f) << ", " << a.At(1.0f) << ", " << a.At(-2.0f) << '\n';
  b.StartDirection({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
  std::cout << b.At(0.0f) << ", " << b.At(1.0f) << ", " << b.At(-2.0f) << '\n';
}

void ClosestPoint()
{
  Math::Ray a, b, c, d, e, f, g, h, i;
  a.StartDirection({1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
  b.StartDirection({1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f});
  c.StartDirection({1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f});
  d.StartDirection({2.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f});
  e.StartDirection({1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, -1.0f});
  f.StartDirection({1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, -1.0f});
  g.StartDirection({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
  h.StartDirection({0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
  i.StartDirection({0.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f});

  std::cout << a.ClosestPointTo(b) << '\n';
  std::cout << a.ClosestPointTo(c) << '\n';
  std::cout << a.ClosestPointTo(d) << '\n';
  std::cout << a.ClosestPointTo(e) << '\n';
  std::cout << a.ClosestPointTo(f) << '\n';

  std::cout << a.ClosestPointTo({3.0f, 3.0f, 3.0f}) << '\n';
  std::cout << g.ClosestPointTo({2.0f, 2.0f, 0.0f}) << '\n';

  std::cout << a.HasClosestTo(c) << a.HasClosestTo(f) << a.HasClosestTo(h)
            << a.HasClosestTo(i) << c.HasClosestTo(d) << '\n';

  // These attempts to find the closest point will signal an abort because h and
  // i are both parallel to a.
  // a.ClosestPointTo(h);
  // a.ClosestPointTo(i);
}

void Intersection()
{
  Math::Ray rays[3];
  Math::Plane planes[3];
  rays[0].StartDirection({0.0, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f});
  rays[1].StartDirection({0.0, 0.0f, 1.0f}, {1.0f, 1.0f, -1.0f});
  rays[2].StartDirection({0.0, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f});
  planes[0].PointNormal({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f});
  planes[1].PointNormal({1.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f});
  planes[2].PointNormal({1.0f, 1.0f, 0.0f}, {1.0f, -1.0f, 0.0f});
  std::cout << Math::Intersection(rays[0], planes[0]) << '\n';
  std::cout << Math::Intersection(rays[1], planes[0]) << '\n';
  std::cout << Math::Intersection(rays[2], planes[0]) << '\n';
  std::cout << Math::Intersection(rays[2], planes[1]) << '\n';

  std::cout << Math::HasIntersection(rays[1], planes[1]) << '\n';
  std::cout << Math::HasIntersection(rays[0], planes[2]) << '\n';

  // This will crash because the ray is perpendicular to the plane normal.
  // Math::Intersection(rays[0], planes[2]);
}

void RaySphere()
{
  Math::Ray rays[3];
  Math::Sphere spheres[2];
  rays[0].StartDirection({0, 0, 0}, {1, 0, 0});
  rays[1].StartDirection({2, 2, 2}, {-1, -1, -1});
  rays[2].StartDirection({-1, 0, -5}, {0, 0, 1});
  spheres[0].Init({0, 0, 0}, 1);
  spheres[1].Init({2, 0, -2}, 2);
  std::cout << Math::Intersection(rays[0], spheres[0]) << '\n';
  std::cout << Math::Intersection(rays[1], spheres[0]) << '\n';
  std::cout << Math::Intersection(rays[0], spheres[1]) << '\n';
  std::cout << Math::Intersection(rays[2], spheres[1]) << '\n';
}

int main(void)
{
  RunTest(At);
  RunTest(ClosestPoint);
  RunTest(Intersection);
  RunTest(RaySphere);
}
