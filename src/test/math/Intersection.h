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
  static Ds::Vector<SphereSphereIntersectionTest> GetTests();
};

struct SphereCapsuleIntersectionTest: BaseTest {
  SphereCapsuleIntersectionTest(
    const std::string& name,
    const Math::Sphere& sphere,
    const Math::Capsule& capsule):
    BaseTest(name), mSphere(sphere), mCapsule(capsule) {}
  Math::Sphere mSphere;
  Math::Capsule mCapsule;
  static Ds::Vector<SphereCapsuleIntersectionTest> GetTests();
};

struct BoxBoxIntersectionTest: BaseTest {
  BoxBoxIntersectionTest(
    const std::string& name, const Math::Box& a, const Math::Box& b):
    BaseTest(name), mA(a), mB(b) {}
  Math::Box mA, mB;
  static Ds::Vector<BoxBoxIntersectionTest> GetTests();
};

struct SphereTriangleIntersectionTest: BaseTest {
  SphereTriangleIntersectionTest(
    const std::string& name,
    const Math::Sphere& sphere,
    const Math::Triangle& triangle):
    BaseTest(name), mSphere(sphere), mTriangle(triangle) {}
  Math::Sphere mSphere;
  Math::Triangle mTriangle;
  static Ds::Vector<SphereTriangleIntersectionTest> GetTests();
};

} // namespace Test

#endif
