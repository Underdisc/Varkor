#include <iostream>

#include "math/Geometry.h"

std::ostream& operator<<(std::ostream& os, const Math::Ray& ray)
{
  os << "{" << ray.mStart << ", " << ray.Direction() << "}";
  return os;
}

void At()
{
  std::cout << "<= At =>" << std::endl;
  Math::Ray a, b;
  a.StartDirection({0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
  std::cout << a.At(0.0f) << ", " << a.At(1.0f) << ", " << a.At(-2.0f)
            << std::endl;
  b.StartDirection({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
  std::cout << b.At(0.0f) << ", " << b.At(1.0f) << ", " << b.At(-2.0f)
            << std::endl
            << std::endl;
}

void ClosestPoint()
{
  std::cout << "<= ClosestPoint =>" << std::endl;

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

  std::cout << a.ClosestPointTo(b) << std::endl;
  std::cout << a.ClosestPointTo(c) << std::endl;
  std::cout << a.ClosestPointTo(d) << std::endl;
  std::cout << a.ClosestPointTo(e) << std::endl;
  std::cout << a.ClosestPointTo(f) << std::endl;

  std::cout << a.ClosestPointTo({3.0f, 3.0f, 3.0f}) << std::endl;
  std::cout << g.ClosestPointTo({2.0f, 2.0f, 0.0f}) << std::endl;

  std::cout << a.HasClosestTo(c) << a.HasClosestTo(f) << a.HasClosestTo(h)
            << a.HasClosestTo(i) << c.HasClosestTo(d) << std::endl
            << std::endl;

  // These attempts to find the closest point will signal an abort because h and
  // i are both parallel to a.
  // a.ClosestPointTo(h);
  // a.ClosestPointTo(i);
}

void Intersection()
{
  std::cout << "<= Intersection =>" << std::endl;
  Math::Ray rays[3];
  Math::Plane planes[3];
  rays[0].StartDirection({0.0, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f});
  rays[1].StartDirection({0.0, 0.0f, 1.0f}, {1.0f, 1.0f, -1.0f});
  rays[2].StartDirection({0.0, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f});
  planes[0].PointNormal({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f});
  planes[1].PointNormal({1.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f});
  planes[2].PointNormal({1.0f, 1.0f, 0.0f}, {1.0f, -1.0f, 0.0f});
  std::cout << Math::Intersection(rays[0], planes[0]) << std::endl;
  std::cout << Math::Intersection(rays[1], planes[0]) << std::endl;
  std::cout << Math::Intersection(rays[2], planes[0]) << std::endl;
  std::cout << Math::Intersection(rays[2], planes[1]) << std::endl;

  std::cout << Math::HasIntersection(rays[1], planes[1]) << std::endl;
  std::cout << Math::HasIntersection(rays[0], planes[2]) << std::endl;

  // This will crash because the ray is perpendicular to the plane normal.
  // Math::Intersection(rays[0], planes[2]);
}

int main(void)
{
  At();
  ClosestPoint();
  Intersection();
}
