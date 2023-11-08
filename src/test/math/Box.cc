#include "math/Box.h"
#include "test/Test.h"
#include "test/math/Box.h"

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

Ds::Vector<BoxBoxIntersectionTest> GetBoxBoxIntersectionTests()
{
  Math::Box a, b;
  Ds::Vector<BoxBoxIntersectionTest> boxIntersectoinTests;

  a = {{0, 0, 0}, {1, 1, 1}, {1, 0, 0, 0}};
  b = {{1, 1, 1}, {1, 1, 1}, {1, 0, 0, 0}};
  boxIntersectoinTests.Emplace("0", a, b);

  a = {{0, 0, 0}, {1, 1, 1}, {1, 0, 0, 0}};
  b = {{0.06f, 1.0803f, 0}, {1, 1, 1}, {1, 0, 0, 0}};
  boxIntersectoinTests.Emplace("1", a, b);

  a = {
    {0.000f, 0.000f, 0.000f},
    {1.000f, 1.000f, 1.000f},
    {1.000f, 0.000f, 0.000f, 0.000f}};
  b = {
    {0.000f, 0.000f, 0.000f},
    {1.000f, 1.000f, 1.000f},
    {1.000f, 0.000f, 0.000f, 0.000f}};
  boxIntersectoinTests.Emplace("2", a, b);

  a = {
    {-0.400f, 0.045f, -0.235f},
    {1.794f, 0.885f, 2.231f},
    {0.911f, 0.181f, 0.346f, -0.133f}};
  b = {
    {1.051f, 0.889f, 1.144f},
    {1.000f, 1.883f, 1.000f},
    {0.720f, 0.000f, 0.000f, 0.693f}};
  boxIntersectoinTests.Emplace("3", a, b);

  a = {
    {-0.400f, 0.045f, -0.235f},
    {1.794f, 0.885f, 2.231f},
    {0.911f, 0.181f, 0.346f, -0.133f}};
  b = {
    {-1.337f, -0.852f, 0.094f},
    {1.000f, 1.883f, 1.000f},
    {0.720f, 0.000f, 0.000f, 0.693f}};
  boxIntersectoinTests.Emplace("4", a, b);

  a = {
    {-3.261f, -0.039f, -1.633f},
    {1.190f, 0.885f, 3.188f},
    {0.868f, 0.224f, 0.365f, -0.250f}};
  b = {
    {-1.337f, -0.852f, 0.094f},
    {1.000f, 1.883f, 1.000f},
    {0.724f, 0.028f, -0.016f, 0.689f}};
  boxIntersectoinTests.Emplace("5", a, b);

  a = {
    {-3.077f, -0.185f, -1.622f},
    {1.190f, 0.885f, 3.188f},
    {0.868f, 0.224f, 0.365f, -0.250f}};
  b = {
    {-1.337f, -0.852f, 0.094f},
    {1.000f, 1.883f, 1.000f},
    {0.724f, 0.028f, -0.016f, 0.689f}};
  boxIntersectoinTests.Emplace("6", a, b);

  a = {
    {-3.077f, -0.185f, -1.622f},
    {1.190f, 0.885f, 3.188f},
    {0.868f, 0.224f, 0.365f, -0.250f}};
  b = {
    {-2.363f, 1.429f, -3.173f},
    {3.082f, 1.021f, 1.177f},
    {0.602f, 0.745f, -0.035f, 0.285f}};
  boxIntersectoinTests.Emplace("7", a, b);

  a = {
    {-2.960f, 0.079f, -1.720f},
    {1.190f, 0.885f, 3.188f},
    {0.868f, 0.224f, 0.365f, -0.250f}};
  b = {
    {-2.363f, 1.429f, -3.173f},
    {3.082f, 1.021f, 1.177f},
    {0.602f, 0.745f, -0.035f, 0.285f}};
  boxIntersectoinTests.Emplace("8", a, b);

  a = {
    {-2.960f, 0.079f, -1.720f},
    {1.190f, 0.885f, 3.188f},
    {0.868f, 0.224f, 0.365f, -0.250f}};
  b = {
    {-2.805f, 1.828f, -2.528f},
    {3.082f, 1.021f, 1.177f},
    {0.349f, 0.878f, -0.325f, -0.033f}};
  boxIntersectoinTests.Emplace("9", a, b);

  a = {
    {-2.960f, 0.079f, -1.720f},
    {1.190f, 0.885f, 3.188f},
    {0.868f, 0.224f, 0.365f, -0.250f}};
  b = {
    {-2.906f, 1.738f, -2.371f},
    {3.082f, 1.021f, 1.177f},
    {0.349f, 0.878f, -0.325f, -0.033f}};
  boxIntersectoinTests.Emplace("10", a, b);

  a = {
    {-2.960f, 0.079f, -1.720f},
    {1.190f, 0.885f, 3.188f},
    {0.868f, 0.224f, 0.365f, -0.250f}};
  b = {
    {-5.262f, -0.471f, -3.008f},
    {3.082f, 1.021f, 1.177f},
    {0.919f, -0.116f, -0.149f, 0.346f}};
  boxIntersectoinTests.Emplace("11", a, b);

  a = {
    {-2.960f, 0.079f, -1.720f},
    {1.190f, 0.885f, 3.188f},
    {0.868f, 0.224f, 0.365f, -0.250f}};
  b = {
    {-5.199f, -0.412f, -2.991f},
    {3.082f, 1.021f, 1.177f},
    {0.919f, -0.116f, -0.149f, 0.346f}};
  boxIntersectoinTests.Emplace("12", a, b);

  a = {
    {-2.379f, -0.117f, -1.869f},
    {1.190f, 0.885f, 3.188f},
    {0.868f, 0.224f, 0.365f, -0.250f}};
  b = {
    {-4.536f, -0.404f, -2.966f},
    {3.082f, 1.021f, 1.177f},
    {0.675f, -0.680f, 0.199f, 0.207f}};
  boxIntersectoinTests.Emplace("13", a, b);

  a = {
    {-2.406f, -0.152f, -1.879f},
    {1.190f, 0.885f, 3.188f},
    {0.868f, 0.224f, 0.365f, -0.250f}};
  b = {
    {-4.536f, -0.404f, -2.966f},
    {3.082f, 1.021f, 1.177f},
    {0.675f, -0.680f, 0.199f, 0.207f}};
  boxIntersectoinTests.Emplace("14", a, b);

  a = {
    {-3.415f, -1.460f, -2.324f},
    {1.190f, 0.885f, 3.188f},
    {-0.388f, 0.570f, 0.180f, 0.702f}};
  b = {
    {-4.536f, -0.404f, -2.966f},
    {3.082f, 1.021f, 1.177f},
    {0.675f, -0.680f, 0.199f, 0.207f}};
  boxIntersectoinTests.Emplace("15", a, b);

  a = {
    {-3.439f, -1.391f, -2.437f},
    {1.190f, 0.885f, 3.188f},
    {-0.388f, 0.570f, 0.180f, 0.702f}};
  b = {
    {-4.536f, -0.404f, -2.966f},
    {3.082f, 1.021f, 1.177f},
    {0.675f, -0.680f, 0.199f, 0.207f}};
  boxIntersectoinTests.Emplace("16", a, b);

  return boxIntersectoinTests;
}

void BoxBoxIntersection()
{
  Ds::Vector<BoxBoxIntersectionTest> tests = GetBoxBoxIntersectionTests();
  for (const BoxBoxIntersectionTest& test : tests) {
    std::cout << test.mName << ": " << Math::HasIntersection(test.mA, test.mB)
              << '\n';
  }
}

} // namespace Test

#ifndef RemoveTestEntryPoint
int main()
{
  using namespace Test;
  RunTest(Support0);
  RunTest(Support1);
  RunTest(BoxBoxIntersection);
}
#endif