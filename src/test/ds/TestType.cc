#include "TestType.h"

int TestType::smDefaultConstructorCount = 0;
int TestType::smCopyConstructorCount = 0;
int TestType::smConstructorCount = 0;
int TestType::smDestructorCount = 0;
int TestType::smCopyEqualsOpCount = 0;
int TestType::smMoveEqualsOpCount = 0;

TestType::TestType()
{
  ++smDefaultConstructorCount;
}

TestType::TestType(const TestType& other): mA(other.mA), mB(other.mB)
{
  ++smCopyConstructorCount;
}

TestType::TestType(int a, float b): mA(a), mB(b)
{
  ++smConstructorCount;
}

TestType::~TestType()
{
  ++smDestructorCount;
}

TestType& TestType::operator=(const TestType& other)
{
  mA = other.mA;
  mB = other.mB;
  ++smCopyEqualsOpCount;
  return *this;
}

TestType& TestType::operator=(TestType&& other)
{
  mA = other.mA;
  mB = other.mB;
  ++smMoveEqualsOpCount;
  return *this;
}

void TestType::ResetCounts()
{
  smDefaultConstructorCount = 0;
  smCopyConstructorCount = 0;
  smConstructorCount = 0;
  smDestructorCount = 0;
  smCopyEqualsOpCount = 0;
  smMoveEqualsOpCount = 0;
}

std::ostream& operator<<(std::ostream& os, const TestType& rhs)
{
  os << "[" << rhs.mA << ", " << rhs.mB << "]";
  return os;
}
