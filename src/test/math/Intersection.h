#ifndef test_math_Intersection_h
#define test_math_Intersection_h

#include "ds/Vector.h"
#include "math/Box.h"
#include "math/Capsule.h"
#include "math/Sphere.h"
#include "math/Triangle.h"
#include "test/Test.h"

namespace Test {

struct SphereSphereIntersectionTest: BaseTest {
  SphereSphereIntersectionTest(
    const std::string& name, const Math::Sphere& a, const Math::Sphere& b):
    BaseTest(name), mA(a), mB(b) {}
  Math::Sphere mA, mB;
};
Ds::Vector<SphereSphereIntersectionTest> GetSphereSphereIntersectionTests();

struct SphereCapsuleIntersectionTest: BaseTest {
  SphereCapsuleIntersectionTest(
    const std::string& name,
    const Math::Sphere& sphere,
    const Math::Capsule& capsule):
    BaseTest(name), mSphere(sphere), mCapsule(capsule) {}
  Math::Sphere mSphere;
  Math::Capsule mCapsule;
};
Ds::Vector<SphereCapsuleIntersectionTest> GetSphereCapsuleIntersectionTests();

struct BoxBoxIntersectionTest: BaseTest {
  BoxBoxIntersectionTest(
    const std::string& name, const Math::Box& a, const Math::Box& b):
    BaseTest(name), mA(a), mB(b) {}
  Math::Box mA, mB;
};
Ds::Vector<BoxBoxIntersectionTest> GetBoxBoxIntersectionTests();

struct SphereTriangleIntersectionTest: BaseTest {
  SphereTriangleIntersectionTest(
    const std::string& name,
    const Math::Sphere& sphere,
    const Math::Triangle& triangle):
    BaseTest(name), mSphere(sphere), mTriangle(triangle) {}
  Math::Sphere mSphere;
  Math::Triangle mTriangle;
};
Ds::Vector<SphereTriangleIntersectionTest> GetSphereTriangleIntersectionTests();

} // namespace Test

#endif
