#include <iostream>

#include "math/Triangle.h"
#include "math/Vector.h"
#include "test/Test.h"
#include "test/math/Triangle.h"

namespace Test {

void BarycentricCoords() {
  Math::Triangle tri = {{{-1, -1, -1}, {1, 0, 1}, {0, 1, 0}}};

  auto printBarycentricCoords = [&](const Vec3& point) {
    std::cout << tri.BarycentricCoords(point) << std::endl;
  };
  const Vec3& a = tri.mPoints[0];
  const Vec3& b = tri.mPoints[1];
  const Vec3& c = tri.mPoints[2];
  printBarycentricCoords(a);
  printBarycentricCoords(b);
  printBarycentricCoords(c);
  printBarycentricCoords({0, 0, 0});
  printBarycentricCoords(a + (b - a) + (c - a));
  printBarycentricCoords(b + (c - b) + (a - b));
  printBarycentricCoords(c + (a - c) + (b - c));
}

Ds::Vector<TriangleClosestPointToTest> TriangleClosestPointToTest::GetTests() {
  Ds::Vector<TriangleClosestPointToTest> tests;
  Math::Triangle triangle;
  Vec3 point;

  triangle = {{{-1, -1, -1}, {1, 0, 1}, {0, 1, 0}}};
  point = {0, 0, 0};
  tests.Emplace("0", triangle, point);

  triangle = {
    {{1.3853f, -1.8971f, -3.9755f},
     {1.5230f, 0.3896f, 5.2937f},
     {1.6027f, 1.6739f, 0.0000f}}};
  point = {1.5780f, 2.0998f, -1.3459f};
  tests.Emplace("1", triangle, point);

  point = {1.5780f, 2.0998f, 0.2941f};
  tests.Emplace("2", triangle, point);

  point = {1.5315f, 2.6821f, -0.1588f};
  tests.Emplace("3", triangle, point);

  point = {1.5315f, 2.2748f, 2.4895f};
  tests.Emplace("4", triangle, point);

  point = {1.5315f, 0.6524f, 5.7707f};
  tests.Emplace("5", triangle, point);

  point = {1.5315f, 0.3768f, 6.7532f};
  tests.Emplace("6", triangle, point);

  point = {1.5315f, -0.3252f, 6.2602f};
  tests.Emplace("7", triangle, point);

  point = {1.5315f, -1.8218f, -1.2194f};
  tests.Emplace("8", triangle, point);

  point = {1.5315f, -2.4426f, -4.1603f};
  tests.Emplace("9", triangle, point);

  point = {1.5315f, -2.1881f, -4.4940f};
  tests.Emplace("10", triangle, point);

  point = {1.5315f, -1.7368f, -4.4671f};
  tests.Emplace("11", triangle, point);

  point = {1.5315f, -1.6220f, -4.1484f};
  tests.Emplace("12", triangle, point);

  point = {1.5315f, 0.0428f, -0.3146f};
  tests.Emplace("13", triangle, point);

  triangle = {
    {{1.5320f, -1.8971f, -0.3838f},
     {-3.0402f, 0.3896f, 2.1171f},
     {-2.7721f, 1.6739f, -4.7477f}}};
  point = {-0.3757f, -3.2775f, 1.9126f};
  tests.Emplace("14", triangle, point);

  triangle = {
    {{-0.7954f, -1.8971f, 2.0919f},
     {-4.2712f, 0.3896f, 0.4309f},
     {-1.9782f, 1.6739f, -2.4372f}}};
  point = {-3.6286f, -3.2775f, -2.5862f};
  tests.Emplace("15", triangle, point);

  triangle = {
    {{-0.9796f, -1.8971f, 3.6106f},
     {-2.8847f, 0.3896f, 0.4309f},
     {-1.9782f, 1.6739f, 2.8848f}}};
  point = {-5.3429f, 3.7865f, 6.9613f};
  tests.Emplace("16", triangle, point);

  return tests;
}

void ClosestPointTo() {
  auto tests = TriangleClosestPointToTest::GetTests();
  for (const TriangleClosestPointToTest& test: tests) {
    std::cout << test.mName << ": "
              << test.mTriangle.ClosestPointTo(test.mPoint) << '\n';
  }
}

} // namespace Test

#ifndef RemoveTestEntryPoint
int main() {
  using namespace Test;
  RunTest(BarycentricCoords);
  RunTest(ClosestPointTo);
  return 0;
}
#endif
