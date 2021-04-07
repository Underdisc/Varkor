#include <iostream>

#include "math/Ray.h"

std::ostream& operator<<(std::ostream& os, const Math::Ray& ray)
{
  os << "{" << ray.mStart << ", " << ray.Direction() << "}";
  return os;
}

void Constructor()
{
  std::cout << "<= Constructor =>" << std::endl;
  Math::Ray a({1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
  Math::Ray b({1.0f, 2.0f, 3.0f}, {1.0f, 1.0f, 1.0f});
  std::cout << a << std::endl << b << std::endl << std::endl;
}

void At()
{
  std::cout << "<= At =>" << std::endl;
  Math::Ray a({0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
  std::cout << a.At(0.0f) << ", " << a.At(1.0f) << ", " << a.At(-2.0f)
            << std::endl;
  Math::Ray b({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
  std::cout << b.At(0.0f) << ", " << b.At(1.0f) << ", " << b.At(-2.0f)
            << std::endl
            << std::endl;
}

void ClosestPoint()
{
  std::cout << "<= ClosestPoint =>" << std::endl;
  Math::Ray a({1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
  Math::Ray b({1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f});
  std::cout << a.ClosestPointTo(b) << std::endl;
  Math::Ray c({1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f});
  std::cout << a.ClosestPointTo(c) << std::endl;
  Math::Ray d({2.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f});
  std::cout << a.ClosestPointTo(d) << std::endl;
  Math::Ray e({1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, -1.0f});
  std::cout << a.ClosestPointTo(e) << std::endl;
  Math::Ray f({1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, -1.0f});
  std::cout << a.ClosestPointTo(f) << std::endl;

  // These attempts to find the closest point will signal an abort because f is
  // parallel to a in both cases.
  // f.Direction({1.0f, 0.0f, 0.0f});
  // a.ClosestPointTo(f);
  // f.Direction({-1.0f, 0.0f, 0.0f});
  // a.ClosestPointTo(f);
}

int main(void)
{
  Constructor();
  At();
  ClosestPoint();
}
