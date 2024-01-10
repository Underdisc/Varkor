#ifndef test_math_Triangle_h
#define test_math_Triangle_h

#include "../../math/Triangle.h"
#include "ds/Vector.h"
#include "test/Test.h"

namespace Test {

struct TriangleClosestPointToTest: BaseTest
{
  TriangleClosestPointToTest(
    const std::string& name, const Math::Triangle& triangle, const Vec3& point):
    BaseTest(name), mTriangle(triangle), mPoint(point)
  {}
  Math::Triangle mTriangle;
  Vec3 mPoint;
};
Ds::Vector<TriangleClosestPointToTest> GetTriangleClosestPointToTests();

} // namespace Test

#endif