#ifndef test_Test_h
#define test_Test_h

#include <string>

namespace Test {

#define TestHeader(function) std::cout << "<= " << #function << " =>\n"

#define RunTest(function) \
  TestHeader(function);   \
  function();             \
  std::cout << std::endl

struct BaseTest
{
  BaseTest(const std::string& name): mName(name) {}
  std::string mName;
};

} // namespace Test

#endif
