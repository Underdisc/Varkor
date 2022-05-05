#include <iostream>
#include <sstream>

#include "test/vlk/Util.h"
#include "vlk/Valkor.h"

void VectorSerialize()
{
  std::cout << "<= VectorSerialize =>\n";
  Vlk::Value root;
  Vec2 vec2 = {0.0f, 1.0f};
  Vec3 vec3 = {0.0f, 1.0f, 2.0f};
  Vec4 vec4 = {0.0f, 1.0f, 2.0f, 3.0f};
  root("vec2") = vec2;
  root("vec3") = vec3;
  root("vec4") = vec4;
  PrintParsePrint(root);
}

void VectorDeserialize()
{
  std::cout << "<= VectorDeserialize =>\n";
  Vlk::Value rootVal;
  rootVal.Read("Vector.vlk");
  Vlk::Explorer rootEx(rootVal);
  Vlk::Explorer vec2Ex = rootEx("vec2");
  std::cout << vec2Ex("0").As<Vec2>({0.0f, 1.0f}) << '\n';
  std::cout << vec2Ex("1").As<Vec2>({2.0f, 3.0f}) << '\n';
  std::cout << vec2Ex("2").As<Vec2>({4.0f, 5.0f}) << '\n';
  Vlk::Explorer vec3Ex = rootEx("vec3");
  std::cout << vec3Ex("0").As<Vec3>({0.0f, 1.0f, 2.0f}) << '\n';
  std::cout << vec3Ex("1").As<Vec3>({3.0f, 4.0f, 5.0f}) << '\n';
  std::cout << vec3Ex("2").As<Vec3>({6.0f, 7.0f, 8.0f}) << '\n';
  std::cout << vec3Ex("3").As<Vec3>({9.0f, 10.0f, 11.0f}) << '\n';
  Vlk::Explorer vec4Ex = rootEx("vec4");
  std::cout << vec4Ex("0").As<Vec4>({0.0f, 1.0f, 2.0f, 3.0f}) << '\n';
  std::cout << vec4Ex("1").As<Vec4>({4.0f, 5.0f, 6.0f, 7.0f}) << '\n';
  std::cout << vec4Ex("2").As<Vec4>({8.0f, 9.0f, 10.0f, 11.0f}) << '\n';
  std::cout << vec4Ex("3").As<Vec4>({12.0f, 13.0f, 14.0f, 15.0f}) << '\n';
  std::cout << vec4Ex("4").As<Vec4>({16.0f, 17.0f, 18.0f, 19.0f}) << '\n';
}

void RunTest(void (*test)())
{
  test();
  std::cout << '\n';
}

int main()
{
  RunTest(VectorSerialize);
  RunTest(VectorDeserialize);
}