#ifndef test_Test_h
#define test_Test_h

#define RunTest(function)                     \
  std::cout << "<= " << #function << " =>\n"; \
  function();                                 \
  std::cout << std::endl;

#endif
