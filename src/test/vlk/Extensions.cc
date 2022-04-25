#include <iostream>
#include <sstream>

#include "test/vlk/Util.h"
#include "vlk/Valkor.h"

void Vec3Serialize()
{
  std::cout << "<= Vec3Serialize =>\n";
  Vlk::Value root;
  Vec3 test[5] = {
    {0.0f, 1.0f, 2.0f},
    {3.0f, 4.0f, 5.0f},
    {6.0f, 7.0f, 8.0f},
    {9.0f, 10.0f, 11.0f},
    {12.0f, 13.0f, 14.0f}};
  for (int i = 0; i < 5; ++i) {
    std::stringstream key;
    key << "test" << i;
    root(key.str()) = test[i];
  }
  PrintParsePrint(root);
}

void Vec3Deserialize()
{
  std::cout << "<= Vec3Deserialize =>\n";
  Vlk::Value rootVal;
  rootVal.Read("Vec3.vlk");
  Vlk::Explorer rootEx(rootVal);
  std::cout << rootEx("test0").As<Vec3>({0.0f, 1.0f, 2.0f}) << '\n';
  std::cout << rootEx("test1").As<Vec3>({3.0f, 4.0f, 5.0f}) << '\n';
  std::cout << rootEx("test2").As<Vec3>({6.0f, 7.0f, 8.0f}) << '\n';
  std::cout << rootEx("test3").As<Vec3>({9.0f, 10.0f, 11.0f}) << '\n';
}

void RunTest(void (*test)())
{
  test();
  std::cout << '\n';
}

int main()
{
  RunTest(Vec3Serialize);
  RunTest(Vec3Deserialize);
}