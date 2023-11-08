#include "math/Box.h"
#include "test/Test.h"

namespace Test {

void Support0()
{
  Math::Box box;
  Vec3 center = {0, 0, 0};
  Vec3 size = {2, 2, 2};
  Quat rotation = {1, 0, 0, 0};
  box.Init(center, size, rotation);
  std::cout << box.Support({1, 1, 1}) << std::endl;
  std::cout << box.Support({-0.5f, 0, 0}) << std::endl;
  std::cout << box.Support({0.5f, -0.5f, -0.5f}) << std::endl;
}

void Support1()
{
  Math::Box box;
  Vec3 center = {1, 1, 0};
  Vec3 size = {4, 2, 4};
  Quat rotation({1, 0, 0}, {0, 1, 0});
  box.Init(center, size, rotation);
  std::cout << box.Support({1, 1, 1}) << std::endl;
  std::cout << box.Support({-0.5f, 0.2f, 0.2f}) << std::endl;
  std::cout << box.Support({0.5f, -0.5f, -0.5f}) << std::endl;
}

} // namespace Test

#ifndef RemoveTestEntryPoint
int main()
{
  using namespace Test;
  RunTest(Support0);
  RunTest(Support1);
}
#endif