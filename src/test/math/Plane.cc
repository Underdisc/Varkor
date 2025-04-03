#include "math/Plane.h"
#include "test/Test.h"

namespace Test {

void Points() {
  Vec3 points[8] = {
    {1, 1, 1},
    {1, 1, -1},
    {-1, 1, -1},
    {-1, 1, 1},
    {1, -1, 1},
    {1, -1, -1},
    {-1, -1, -1},
    {-1, -1, 1}};
  int triangleIdxs[4][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 1}, {0, 2, 7}};
  for (int i = 0; i < 4; ++i) {
    int* idxs = triangleIdxs[i];
    Math::Plane plane =
      Math::Plane::Points(points[idxs[0]], points[idxs[1]], points[idxs[2]]);
    std::cout << plane.mPoint << ", " << plane.Normal() << '\n';
  }
}

} // namespace Test

#ifndef RemoveTestEntryPoint
int main() {
  using namespace Test;
  RunTest(Points);
}
#endif
