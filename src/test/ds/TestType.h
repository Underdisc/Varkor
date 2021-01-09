#ifndef test_ds_TestType_h
#define test_ds_TestType_h

#include <ostream>

struct TestType
{
  TestType();
  TestType(const TestType& other);
  TestType(int a, float b);
  ~TestType();
  TestType& operator=(const TestType& other);
  TestType& operator=(TestType&& other);
  static void ResetCounts();

  static int smDefaultConstructorCount;
  static int smCopyConstructorCount;
  static int smConstructorCount;
  static int smDestructorCount;
  static int smCopyEqualsOpCount;
  static int smMoveEqualsOpCount;

  int mA;
  float mB;
};

std::ostream& operator<<(std::ostream& os, const TestType& rhs);

#endif
