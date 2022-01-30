#include <iostream>

#include "TestType.h"

int TestType::smDefaultConstructorCount = 0;
int TestType::smConstructorCount = 0;
int TestType::smCopyConstructorCount = 0;
int TestType::smMoveConstructorCount = 0;
int TestType::smDestructorCount = 0;
int TestType::smCopyAssignmentCount = 0;
int TestType::smMoveAssignmentCount = 0;

TestType::TestType()
{
  ++smDefaultConstructorCount;
}

TestType::TestType(int value)
{
  ++smConstructorCount;
  Set(value);
}

TestType::TestType(int a, float b): mA(a), mB(b)
{
  ++smConstructorCount;
}

TestType::TestType(const TestType& other): mA(other.mA), mB(other.mB)
{
  ++smCopyConstructorCount;
}

TestType::TestType(TestType&& other): mA(other.mA), mB(other.mB)
{
  ++smMoveConstructorCount;
}

TestType::~TestType()
{
  ++smDestructorCount;
}

void TestType::Set(int value)
{
  mA = value;
  mB = (float)value;
}

TestType& TestType::operator=(const TestType& other)
{
  mA = other.mA;
  mB = other.mB;
  ++smCopyAssignmentCount;
  return *this;
}

TestType& TestType::operator=(TestType&& other)
{
  mA = other.mA;
  mB = other.mB;
  ++smMoveAssignmentCount;
  return *this;
}

bool TestType::operator>(const TestType& other)
{
  return mA > other.mA;
}

bool TestType::operator<(const TestType& other)
{
  return mA < other.mA;
}

void TestType::ResetCounts()
{
  smDefaultConstructorCount = 0;
  smConstructorCount = 0;
  smCopyConstructorCount = 0;
  smMoveConstructorCount = 0;
  smDestructorCount = 0;
  smCopyAssignmentCount = 0;
  smMoveAssignmentCount = 0;
}

void TestType::PrintCounts()
{
  std::cout << "-Counts-\n"
            << "DefaultConstructor: " << smDefaultConstructorCount << "\n"
            << "Constructor: " << smConstructorCount << "\n"
            << "CopyConstructor: " << smCopyConstructorCount << "\n"
            << "MoveConstructor: " << smMoveConstructorCount << "\n"
            << "Destructor: " << smDestructorCount << "\n"
            << "CopyAssignment: " << smCopyAssignmentCount << "\n"
            << "MoveAssignment: " << smMoveAssignmentCount << "\n";
}

std::ostream& operator<<(std::ostream& os, const TestType& rhs)
{
  os << "[" << rhs.mA << ", " << rhs.mB << "]";
  return os;
}
