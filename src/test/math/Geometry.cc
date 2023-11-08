#include <iostream>

#include "math/Geometry.h"
#include "test/Test.h"
#include "test/math/Geometry.h"

namespace Test {

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

Ds::Vector<SphereSphereIntersectionTest> GetSphereSphereIntersectionTests()
{
  Ds::Vector<SphereSphereIntersectionTest> tests;
  Math::Sphere a, b;

  a = {{0, 0, 0}, 1};
  b = {{0, 0, 0}, 1};
  tests.Emplace("0", a, b);

  a = {{-1.3021f, 0.0000f, -0.1861f}, 1.0000f};
  b = {{0.0000f, 0.0000f, 0.0000f}, 1.0000f};
  tests.Emplace("1", a, b);

  a = {{-1.3021f, 0.0000f, -0.1861f}, 1.0000f};
  b = {{0.4662f, 0.4902f, -0.3273f}, 0.7095f};
  tests.Emplace("2", a, b);

  a = {{0.0658f, 0.0000f, 1.4832f}, 1.4719f};
  b = {{0.4662f, 0.4902f, -0.3273f}, 0.7095f};
  tests.Emplace("3", a, b);

  a = {{-0.2620f, -0.0000f, 1.2747f}, 1.4719f};
  b = {{0.6807f, 0.4902f, -1.7491f}, 2.5317f};
  tests.Emplace("4", a, b);

  a = {{1.7430f, -3.2200f, -0.4813f}, 1.4719f};
  b = {{1.0906f, 0.8162f, -0.7119f}, 2.5347f};
  tests.Emplace("5", a, b);

  a = {{1.4530f, 4.1771f, -0.7421f}, 1.4719f};
  b = {{1.0906f, 0.8162f, -0.7119f}, 2.5347f};
  tests.Emplace("6", a, b);

  return tests;
}

void SphereSphereIntersection()
{
  const auto tests = GetSphereSphereIntersectionTests();
  for (const SphereSphereIntersectionTest& test : tests) {
    Math::SphereSphere result = Math::Intersection(test.mA, test.mB);
    std::cout << test.mName << ": " << result.mIntersecting << ", "
              << result.mSeparation << "\n";
  }
}

} // namespace Test

#ifndef RemoveTestEntryPoint
int main(void)
{
  using namespace Test;
  RunTest(At);
  RunTest(ClosestPoint);
  RunTest(Intersection);
  RunTest(RaySphere);
  RunTest(SphereSphereIntersection);
}
#endif
