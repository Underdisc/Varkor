#ifndef test_math_Box_h
#define test_math_Box_h

#include "../../math/Box.h"
#include "ds/Vector.h"

namespace Test {

struct BoxIntersectionTest
{
  BoxIntersectionTest(
    const std::string& name, const Math::Box& a, const Math::Box& b):
    mName(name), mA(a), mB(b)
  {}
  std::string mName;
  Math::Box mA, mB;
};

Ds::Vector<BoxIntersectionTest> GetBoxIntersectionTests();

} // namespace Test

#endif