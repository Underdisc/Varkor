#include <iostream>

#include "math/Ray.h"
#include "test/Test.h"

namespace Test {

void At() {
  Math::Ray a = Math::Ray::StartDirection({0, 1, 0}, {1, 0, 0});
  std::cout << a.At(0.0f) << ", " << a.At(1.0f) << ", " << a.At(-2.0f) << '\n';
  Math::Ray b = Math::Ray::StartNormalizeDirection({0, 0, 0}, {1, 1, 1});
  std::cout << b.At(0.0f) << ", " << b.At(1.0f) << ", " << b.At(-2.0f) << '\n';
}

void ClosestPoint() {
  Math::Ray a, b, c, d, e, f, g, h, i;
  a = Math::Ray::StartDirection({1, 0, 0}, {1, 0, 0});
  b = Math::Ray::StartDirection({1, 0, 1}, {0, 0, 1});
  c = Math::Ray::StartDirection({1, 1, 1}, {0, 0, 1});
  d = Math::Ray::StartDirection({2, 1, 1}, {0, 0, 1});
  e = Math::Ray::StartNormalizeDirection({1, 1, 1}, {1, 0, -1});
  f = Math::Ray::StartNormalizeDirection({1, 1, 1}, {1, 1, -1});
  g = Math::Ray::StartNormalizeDirection({0, 0, 0}, {1, 1, 1});
  h = Math::Ray::StartDirection({0, 1, 0}, {1, 0, 0});
  i = Math::Ray::StartDirection({0, 0, 1}, {-1, 0, 0});

  std::cout << a.ClosestPointTo(b) << '\n';
  std::cout << a.ClosestPointTo(c) << '\n';
  std::cout << a.ClosestPointTo(d) << '\n';
  std::cout << a.ClosestPointTo(e) << '\n';
  std::cout << a.ClosestPointTo(f) << '\n';

  std::cout << a.ClosestPointTo(Vec3 {3.0f, 3.0f, 3.0f}) << '\n';
  std::cout << g.ClosestPointTo(Vec3 {2.0f, 2.0f, 0.0f}) << '\n';

  std::cout << a.HasClosestTo(c) << a.HasClosestTo(f) << a.HasClosestTo(h)
            << a.HasClosestTo(i) << c.HasClosestTo(d) << '\n';

  // These attempts to find the closest point will signal an abort because h and
  // i are both parallel to a.
  // a.ClosestPointTo(h);
  // a.ClosestPointTo(i);
}

} // namespace Test

int main(void) {
  using namespace Test;
  RunTest(At);
  RunTest(ClosestPoint);
}
