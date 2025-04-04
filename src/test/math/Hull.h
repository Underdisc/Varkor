#ifndef test_math_Mesh_h
#define test_math_Mesh_h

#include "ds/Vector.h"
#include "math/Vector.h"
#include "test/Test.h"

namespace Test {

struct QuickHullTest: BaseTest {
  QuickHullTest(const std::string& name, Ds::Vector<Vec3>&& points):
    BaseTest(name), mPoints(std::move(points)) {}
  Ds::Vector<Vec3> mPoints;
  static Ds::Vector<QuickHullTest> GetTests();
};

} // namespace Test

#endif
