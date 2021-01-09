#ifndef test_ds_TestType_h
#define test_ds_TestType_h

#include <ostream>

struct TestType
{
  TestType();
  TestType(int a, float b);
  TestType(const TestType& other);
  TestType(TestType&& other);
  ~TestType();
  TestType& operator=(const TestType& other);
  TestType& operator=(TestType&& other);
  bool operator>(const TestType& other);
  bool operator<(const TestType& other);
  static void ResetCounts();

  static int smDefaultConstructorCount;
  static int smConstructorCount;
  static int smCopyConstructorCount;
  static int smMoveConstructorCount;
  static int smDestructorCount;
  static int smCopyEqualsOpCount;
  static int smMoveEqualsOpCount;

  int mA;
  float mB;
};

std::ostream& operator<<(std::ostream& os, const TestType& rhs);

#endif
