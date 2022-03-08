#include <iostream>

#include "util/Delegate.h"

void Basic()
{
  std::cout << "Basic" << std::endl;
}

void Args(int i, float f)
{
  std::cout << "Args " << i << " " << f << std::endl;
}

int ReturnAndArgs(int i, float f)
{
  std::cout << "ReturnAndArgs " << i << " " << f;
  return i + 2;
}

void FreeFunction()
{
  std::cout << "<= FreeFunction =>" << std::endl;
  Util::Delegate<void> basic;
  basic.Bind<Basic>();
  basic.Invoke();

  Util::Delegate<void, int, float> args;
  args.Bind<Args>();
  args.Invoke(1, 2);

  Util::Delegate<int, int, float> returnAndArgs;
  returnAndArgs.Bind<ReturnAndArgs>();
  int returnValue = returnAndArgs.Invoke(1, 2);
  std::cout << " " << returnValue << std::endl
            << "Open: " << basic.Open() << args.Open() << returnAndArgs.Open()
            << std::endl
            << std::endl;
}

void MemberFunction()
{
  std::cout << "<= MemberFunction =>" << std::endl;
  struct Object
  {
    int mValue;
    Object(): mValue(0) {}
    void Basic()
    {
      ++mValue;
      std::cout << mValue << " Basic" << std::endl;
    }
    void Args(int i, float f)
    {
      ++mValue;
      std::cout << mValue << " Args " << i << " " << f << std::endl;
    }
    int ReturnAndArgs(int i, float f)
    {
      ++mValue;
      std::cout << mValue << " ReturnAndArgs " << i << " " << f;
      return i + 2;
    }
  };
  Object object;
  Util::Delegate<void> basic;
  basic.Bind<Object, &Object::Basic>();
  basic.Invoke((void*)&object);

  Util::Delegate<void, int, float> args;
  args.Bind<Object, &Object::Args>();
  args.Invoke((void*)&object, 1, 2);

  Util::Delegate<int, int, float> returnAndArgs;
  returnAndArgs.Bind<Object, &Object::ReturnAndArgs>();
  int returnValue = returnAndArgs.Invoke((void*)&object, 1, 2);
  std::cout << " " << returnValue << std::endl
            << "Open: " << basic.Open() << args.Open() << returnAndArgs.Open()
            << std::endl
            << std::endl;
}

void NullFunction()
{
  std::cout << "<= NullFunction =>" << std::endl;
  Util::Delegate<void> test;
  test.BindNull();
  std::cout << "Open: " << test.Open() << std::endl << std::endl;
}

int main(void)
{
  FreeFunction();
  MemberFunction();
  NullFunction();
}
