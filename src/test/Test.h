#ifndef test_Test_h
#define test_Test_h

#include <string>

namespace Test {

#define RunTest(function)                     \
  std::cout << "<= " << #function << " =>\n"; \
  function();                                 \
  std::cout << std::endl;

struct BaseTest
{
  BaseTest(const std::string& name): mName(name) {}
  std::string mName;
};

} // namespace Test

#endif
