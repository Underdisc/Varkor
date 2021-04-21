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

  // These tests are for finding the closest points on two Rays.
  Math::Ray a, b, c, d, e, f, g;
  a.StartDirection({1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
  b.StartDirection({1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f});
  c.StartDirection({1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f});
  d.StartDirection({2.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f});
  e.StartDirection({1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, -1.0f});
  f.StartDirection({1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, -1.0f});
  g.StartDirection({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
  std::cout << a.ClosestPointTo(b) << std::endl;
  std::cout << a.ClosestPointTo(c) << std::endl;
  std::cout << a.ClosestPointTo(d) << std::endl;
  std::cout << a.ClosestPointTo(e) << std::endl;
  std::cout << a.ClosestPointTo(f) << std::endl;

  // These tests are for finding the point on a Ray that is closest to a point.
  std::cout << a.ClosestPointTo({3.0f, 3.0f, 3.0f}) << std::endl;
  std::cout << g.ClosestPointTo({2.0f, 2.0f, 0.0f}) << std::endl << std::endl;

  // These attempts to find the closest point will signal an abort because f is
  // parallel to a in both cases.
  // f.Direction({1.0f, 0.0f, 0.0f});
  // a.ClosestPointTo(f);
  // f.Direction({-1.0f, 0.0f, 0.0f});
  // a.ClosestPointTo(f);
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
