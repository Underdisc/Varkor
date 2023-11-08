#ifndef test_math_Geometry_h
#define test_math_Geometry_h

#include "math/Sphere.h"
#include "ds/Vector.h"
#include "test/Test.h"

namespace Test {

struct SphereSphereIntersectionTest: BaseTest
{
  SphereSphereIntersectionTest(
    const std::string& name, const Math::Sphere& a, const Math::Sphere& b):
    BaseTest(name), mA(a), mB(b)
  {}
  Math::Sphere mA, mB;
};
Ds::Vector<SphereSphereIntersectionTest> GetSphereSphereIntersectionTests();

} // namespace Test

#endif