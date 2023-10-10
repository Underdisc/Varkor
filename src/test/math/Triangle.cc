#include <iostream>

#include "math/Triangle.h"
#include "math/Vector.h"
#include "test/Test.h"

void BarycentricCoords()
{
  Vec3 triangle[3];
  triangle[0] = {-1, -1, -1};
  triangle[1] = {1, 0, 1};
  triangle[2] = {0, 1, 0};
  const Vec3* tri = triangle;

  auto printBarycentricCoords = [&](const Vec3& point)
  {
    Vec3 barycentricCoords =
      Math::BarycentricCoords(tri[0], tri[1], tri[2], point);
    std::cout << barycentricCoords << std::endl;
  };
  printBarycentricCoords(tri[0]);
  printBarycentricCoords(tri[1]);
  printBarycentricCoords(tri[2]);
  printBarycentricCoords({0, 0, 0});
  printBarycentricCoords(tri[0] + (tri[1] - tri[0]) + (tri[2] - tri[0]));
  printBarycentricCoords(tri[1] + (tri[2] - tri[1]) + (tri[0] - tri[1]));
  printBarycentricCoords(tri[2] + (tri[0] - tri[2]) + (tri[1] - tri[2]));
}

void main()
{
  RunTest(BarycentricCoords);
}