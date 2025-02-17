#include <iostream>

#include "math/Ray.h"
#include "test/Test.h"

namespace Test {

void At()
{
  Math::Ray a, b;
  a.StartDirection({0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
  std::cout << a.At(0.0f) << ", " << a.At(1.0f) << ", " << a.At(-2.0f) << '\n';
  b.StartDirection({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
  std::cout << b.At(0.0f) << ", " << b.At(1.0f) << ", " << b.At(-2.0f) << '\n';
}

void ClosestPoint()
{
  Math::Ray a, b, c, d, e, f, g, h, i;
  a.StartDirection({1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
  b.StartDirection({1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f});
  c.StartDirection({1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f});
  d.StartDirection({2.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f});
  e.StartDirection({1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, -1.0f});
  f.StartDirection({1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, -1.0f});
  g.StartDirection({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
  h.StartDirection({0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
  i.StartDirection({0.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f});

  std::cout << a.ClosestPointTo(b) << '\n';
  std::cout << a.ClosestPointTo(c) << '\n';
  std::cout << a.ClosestPointTo(d) << '\n';
  std::cout << a.ClosestPointTo(e) << '\n';
  std::cout << a.ClosestPointTo(f) << '\n';

  std::cout << a.ClosestPointTo({3.0f, 3.0f, 3.0f}) << '\n';
  std::cout << g.ClosestPointTo({2.0f, 2.0f, 0.0f}) << '\n';

  std::cout << a.HasClosestTo(c) << a.HasClosestTo(f) << a.HasClosestTo(h)
            << a.HasClosestTo(i) << c.HasClosestTo(d) << '\n';

  // These attempts to find the closest point will signal an abort because h and
  // i are both parallel to a.
  // a.ClosestPointTo(h);
  // a.ClosestPointTo(i);
}

} // namespace Test

int main(void)
{
  using namespace Test;
  RunTest(At);
  RunTest(ClosestPoint);
}