#include <iostream>

#include "util/Delegate.h"

void DelegateFree()
{
  std::cout << "Free" << std::endl;
}

struct Object
{
  int mValue;
  void DelegateMember()
  {
    std::cout << "Member " << mValue << std::endl;
  }
};

void FreeFunction()
{
  std::cout << "<= FreeFunction =>" << std::endl;
  Util::Delegate test;
  test.Bind<DelegateFree>();
  std::cout << test.Open() << std::endl;
  test.Invoke();
  std::cout << std::endl;
}

void MemberFunction()
{
  std::cout << "<= MemberFunction =>" << std::endl;
  Util::Delegate test;
  test.Bind<Object, &Object::DelegateMember>();
  std::cout << test.Open() << std::endl;

  Object object;
  object.mValue = 5;
  test.Invoke((void*)&object);
  std::cout << std::endl;
}

void NullFunction()
{
  std::cout << "<= NullFunction =>" << std::endl;
  Util::Delegate test;
  test.BindNull();
  std::cout << test.Open() << std::endl << std::endl;
}

int main(void)
{
  FreeFunction();
  MemberFunction();
  NullFunction();
}
