#ifndef test_math_Box_h
#define test_math_Box_h

#include "../../math/Box.h"
#include "ds/Vector.h"
#include "test/Test.h"

namespace Test {

struct BoxBoxIntersectionTest: BaseTest
{
  BoxBoxIntersectionTest(
    const std::string& name, const Math::Box& a, const Math::Box& b):
    BaseTest(name), mA(a), mB(b)
  {}
  Math::Box mA, mB;
};

Ds::Vector<BoxBoxIntersectionTest> GetBoxBoxIntersectionTests();

} // namespace Test

#endif