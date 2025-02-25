#include <iostream>

#include "math/Intersection.h"
#include "test/Test.h"
#include "test/math/Intersection.h"

namespace Test {

std::ostream& operator<<(std::ostream& os, const Math::RayPlane& intersection) {
  os << intersection.mIntersecting;
  if (intersection.mIntersecting) {
    os << ": " << intersection.mIntersection;
  }
  return os;
}

std::ostream& operator<<(
  std::ostream& os, const Math::RaySphere& intersection) {
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

void Intersection() {
  Math::Ray rays[3];
  Math::Plane planes[3];
  rays[0].StartDirection({0.0, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f});
  rays[1].StartDirection({0.0, 0.0f, 1.0f}, {1.0f, 1.0f, -1.0f});
  rays[2].StartDirection({0.0, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f});
  planes[0].PointNormal({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f});
  planes[1].PointNormal({1.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f});
  planes[2].PointNormal({1.0f, 1.0f, 0.0f}, {1.0f, -1.0f, 0.0f});
  std::cout << Math::Intersection(rays[0], planes[0]) << '\n';
  std::cout << Math::Intersection(rays[0], planes[2]) << '\n';
  std::cout << Math::Intersection(rays[1], planes[0]) << '\n';
  std::cout << Math::Intersection(rays[1], planes[1]) << '\n';
  std::cout << Math::Intersection(rays[2], planes[0]) << '\n';
  std::cout << Math::Intersection(rays[2], planes[1]) << '\n';
}

void RaySphere() {
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

Ds::Vector<SphereSphereIntersectionTest> GetSphereSphereIntersectionTests() {
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

void SphereSphereIntersection() {
  const auto tests = GetSphereSphereIntersectionTests();
  for (const SphereSphereIntersectionTest& test: tests) {
    Math::SphereSphere result = Math::Intersection(test.mA, test.mB);
    std::cout << test.mName << ": " << result.mIntersecting;
    if (result.mIntersecting) {
      std::cout << ", " << result.mNormal << ", " << result.mContactPoint
                << ", " << result.mPenetration;
    }
    std::cout << "\n";
  }
}

Ds::Vector<SphereCapsuleIntersectionTest> GetSphereCapsuleIntersectionTests() {
  Ds::Vector<SphereCapsuleIntersectionTest> tests;
  Math::Sphere sphere;
  Math::Capsule capsule;
  sphere = {{5, 5, 5}, 1};
  capsule = {{{0, 0, 0}, {1, 1, 1}}, 1};
  tests.Emplace("0", sphere, capsule);
  return tests;
}

Ds::Vector<BoxBoxIntersectionTest> GetBoxBoxIntersectionTests() {
  Math::Box a, b;
  Ds::Vector<BoxBoxIntersectionTest> boxIntersectoinTests;

  a = {{0, 0, 0}, {1, 1, 1}, {1, 0, 0, 0}};
  b = {{1, 1, 1}, {1, 1, 1}, {1, 0, 0, 0}};
  boxIntersectoinTests.Emplace("0", a, b);

  a = {{0, 0, 0}, {1, 1, 1}, {1, 0, 0, 0}};
  b = {{0.06f, 1.0803f, 0}, {1, 1, 1}, {1, 0, 0, 0}};
  boxIntersectoinTests.Emplace("1", a, b);

  a = {
    {0.000f, 0.000f, 0.000f},
    {1.000f, 1.000f, 1.000f},
    {1.000f, 0.000f, 0.000f, 0.000f}};
  b = {
    {0.000f, 0.000f, 0.000f},
    {1.000f, 1.000f, 1.000f},
    {1.000f, 0.000f, 0.000f, 0.000f}};
  boxIntersectoinTests.Emplace("2", a, b);

  a = {
    {-0.400f, 0.045f, -0.235f},
    {1.794f, 0.885f, 2.231f},
    {0.911f, 0.181f, 0.346f, -0.133f}};
  b = {
    {1.051f, 0.889f, 1.144f},
    {1.000f, 1.883f, 1.000f},
    {0.720f, 0.000f, 0.000f, 0.693f}};
  boxIntersectoinTests.Emplace("3", a, b);

  a = {
    {-0.400f, 0.045f, -0.235f},
    {1.794f, 0.885f, 2.231f},
    {0.911f, 0.181f, 0.346f, -0.133f}};
  b = {
    {-1.337f, -0.852f, 0.094f},
    {1.000f, 1.883f, 1.000f},
    {0.720f, 0.000f, 0.000f, 0.693f}};
  boxIntersectoinTests.Emplace("4", a, b);

  a = {
    {-3.261f, -0.039f, -1.633f},
    {1.190f, 0.885f, 3.188f},
    {0.868f, 0.224f, 0.365f, -0.250f}};
  b = {
    {-1.337f, -0.852f, 0.094f},
    {1.000f, 1.883f, 1.000f},
    {0.724f, 0.028f, -0.016f, 0.689f}};
  boxIntersectoinTests.Emplace("5", a, b);

  a = {
    {-3.077f, -0.185f, -1.622f},
    {1.190f, 0.885f, 3.188f},
    {0.868f, 0.224f, 0.365f, -0.250f}};
  b = {
    {-1.337f, -0.852f, 0.094f},
    {1.000f, 1.883f, 1.000f},
    {0.724f, 0.028f, -0.016f, 0.689f}};
  boxIntersectoinTests.Emplace("6", a, b);

  a = {
    {-3.077f, -0.185f, -1.622f},
    {1.190f, 0.885f, 3.188f},
    {0.868f, 0.224f, 0.365f, -0.250f}};
  b = {
    {-2.363f, 1.429f, -3.173f},
    {3.082f, 1.021f, 1.177f},
    {0.602f, 0.745f, -0.035f, 0.285f}};
  boxIntersectoinTests.Emplace("7", a, b);

  a = {
    {-2.960f, 0.079f, -1.720f},
    {1.190f, 0.885f, 3.188f},
    {0.868f, 0.224f, 0.365f, -0.250f}};
  b = {
    {-2.363f, 1.429f, -3.173f},
    {3.082f, 1.021f, 1.177f},
    {0.602f, 0.745f, -0.035f, 0.285f}};
  boxIntersectoinTests.Emplace("8", a, b);

  a = {
    {-2.960f, 0.079f, -1.720f},
    {1.190f, 0.885f, 3.188f},
    {0.868f, 0.224f, 0.365f, -0.250f}};
  b = {
    {-2.805f, 1.828f, -2.528f},
    {3.082f, 1.021f, 1.177f},
    {0.349f, 0.878f, -0.325f, -0.033f}};
  boxIntersectoinTests.Emplace("9", a, b);

  a = {
    {-2.960f, 0.079f, -1.720f},
    {1.190f, 0.885f, 3.188f},
    {0.868f, 0.224f, 0.365f, -0.250f}};
  b = {
    {-2.906f, 1.738f, -2.371f},
    {3.082f, 1.021f, 1.177f},
    {0.349f, 0.878f, -0.325f, -0.033f}};
  boxIntersectoinTests.Emplace("10", a, b);

  a = {
    {-2.960f, 0.079f, -1.720f},
    {1.190f, 0.885f, 3.188f},
    {0.868f, 0.224f, 0.365f, -0.250f}};
  b = {
    {-5.262f, -0.471f, -3.008f},
    {3.082f, 1.021f, 1.177f},
    {0.919f, -0.116f, -0.149f, 0.346f}};
  boxIntersectoinTests.Emplace("11", a, b);

  a = {
    {-2.960f, 0.079f, -1.720f},
    {1.190f, 0.885f, 3.188f},
    {0.868f, 0.224f, 0.365f, -0.250f}};
  b = {
    {-5.199f, -0.412f, -2.991f},
    {3.082f, 1.021f, 1.177f},
    {0.919f, -0.116f, -0.149f, 0.346f}};
  boxIntersectoinTests.Emplace("12", a, b);

  a = {
    {-2.379f, -0.117f, -1.869f},
    {1.190f, 0.885f, 3.188f},
    {0.868f, 0.224f, 0.365f, -0.250f}};
  b = {
    {-4.536f, -0.404f, -2.966f},
    {3.082f, 1.021f, 1.177f},
    {0.675f, -0.680f, 0.199f, 0.207f}};
  boxIntersectoinTests.Emplace("13", a, b);

  a = {
    {-2.406f, -0.152f, -1.879f},
    {1.190f, 0.885f, 3.188f},
    {0.868f, 0.224f, 0.365f, -0.250f}};
  b = {
    {-4.536f, -0.404f, -2.966f},
    {3.082f, 1.021f, 1.177f},
    {0.675f, -0.680f, 0.199f, 0.207f}};
  boxIntersectoinTests.Emplace("14", a, b);

  a = {
    {-3.415f, -1.460f, -2.324f},
    {1.190f, 0.885f, 3.188f},
    {-0.388f, 0.570f, 0.180f, 0.702f}};
  b = {
    {-4.536f, -0.404f, -2.966f},
    {3.082f, 1.021f, 1.177f},
    {0.675f, -0.680f, 0.199f, 0.207f}};
  boxIntersectoinTests.Emplace("15", a, b);

  a = {
    {-3.439f, -1.391f, -2.437f},
    {1.190f, 0.885f, 3.188f},
    {-0.388f, 0.570f, 0.180f, 0.702f}};
  b = {
    {-4.536f, -0.404f, -2.966f},
    {3.082f, 1.021f, 1.177f},
    {0.675f, -0.680f, 0.199f, 0.207f}};
  boxIntersectoinTests.Emplace("16", a, b);

  return boxIntersectoinTests;
}

void BoxBoxIntersection() {
  Ds::Vector<BoxBoxIntersectionTest> tests = GetBoxBoxIntersectionTests();
  for (const BoxBoxIntersectionTest& test: tests) {
    std::cout << test.mName << ": " << Math::HasIntersection(test.mA, test.mB)
              << '\n';
  }
}

Ds::Vector<SphereTriangleIntersectionTest>
GetSphereTriangleIntersectionTests() {
  Ds::Vector<SphereTriangleIntersectionTest> tests;
  Math::Sphere sphere = {{0, 0, 0}, 1};
  Math::Triangle triangle = {{{-1, -1, 0}, {1, 0, 0}, {0, 1, 0}}};
  tests.Emplace("0", sphere, triangle);

  sphere = {{-1.1826f, -2.1240f, 0.0000f}, 1.0000f};
  triangle = {
    {{-1.0000f, -1.0000f, 0.0000f},
     {1.0000f, 0.0000f, 0.0000f},
     {0.0000f, 1.0000f, 0.0000f}}};
  tests.Emplace("1", sphere, triangle);

  sphere = {{-0.3490f, 2.2126f, -11.0209f}, 7.1703f};
  triangle = {
    {{-1.0000f, -1.0000f, 1.6633f},
     {-0.5540f, 0.0000f, -2.7632f},
     {-0.7289f, 1.6005f, 0.0000f}}};
  tests.Emplace("2", sphere, triangle);

  sphere = {{-0.6043f, 4.4224f, -5.9788f}, 5.6594f};
  triangle = {
    {{-6.0899f, 4.0454f, 4.2461f},
     {7.6547f, -2.9160f, 7.7353f},
     {-0.5894f, 2.7536f, -1.0266f}}};
  tests.Emplace("3", sphere, triangle);

  sphere = {{-4.4862f, 0.9700f, 0.6243f}, 3.4907f};
  tests.Emplace("4", sphere, triangle);

  sphere = {{-8.5101f, 7.5530f, 4.4012f}, 4.9970f};
  tests.Emplace("5", sphere, triangle);

  sphere = {{0.9213f, 0.7249f, 6.6503f}, 0.7686f};
  tests.Emplace("6", sphere, triangle);

  sphere = {{9.4665f, -2.8856f, 10.0305f}, 2.1800f};
  tests.Emplace("7", sphere, triangle);

  sphere = {{8.3771f, -2.8856f, 7.4411f}, 0.9005f};
  tests.Emplace("8", sphere, triangle);

  sphere = {{3.9089f, -2.8856f, -1.4791f}, 4.0346f};
  tests.Emplace("9", sphere, triangle);

  sphere = {{-0.9630f, -4.5119f, 2.6363f}, 4.0346f};
  tests.Emplace("10", sphere, triangle);

  sphere = {{1.3387f, 4.6699f, 4.4361f}, 4.5919f};
  tests.Emplace("11", sphere, triangle);

  sphere = {{1.3212f, 1.0032f, 5.2379f}, 3.8955f};
  triangle = {
    {{-8.3002f, -5.3403f, 0.9801f},
     {4.6298f, -2.9160f, 10.3280f},
     {-3.0387f, 2.7536f, 8.9678f}}};
  tests.Emplace("12", sphere, triangle);

  return tests;
}

void SphereTriangleIntersection() {
  Ds::Vector<SphereTriangleIntersectionTest> tests =
    GetSphereTriangleIntersectionTests();
  for (const SphereTriangleIntersectionTest& test: tests) {
    Math::SphereTriangle intersection =
      Math::Intersection(test.mSphere, test.mTriangle);
    std::cout << test.mName << ": " << intersection.mIntersecting;
    if (intersection.mIntersecting) {
      std::cout << ", " << intersection.mSeparation;
    }
    std::cout << "\n";
  }
}

} // namespace Test

#ifndef RemoveTestEntryPoint
int main(void) {
  using namespace Test;
  RunTest(Intersection);
  RunTest(RaySphere);
  RunTest(SphereSphereIntersection);
  RunTest(BoxBoxIntersection);
  RunTest(SphereTriangleIntersection);
}
#endif
