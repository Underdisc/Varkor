#include <iostream>

#include "test/Test.h"
#include "util/Delegate.h"

void Basic()
{
  std::cout << "Basic\n";
}

void Args(int i, float f)
{
  std::cout << "Args " << i << " " << f << '\n';
}

int ReturnAndArgs(int i, float f)
{
  std::cout << "ReturnAndArgs " << i << " " << f;
  return i + 2;
}

void FreeFunction()
{
  Util::Delegate<void> basic;
  basic.Bind<Basic>();
  basic.Invoke();

  Util::Delegate<void, int, float> args;
  args.Bind<Args>();
  args.Invoke(1, 2);

  Util::Delegate<int, int, float> returnAndArgs;
  returnAndArgs.Bind<ReturnAndArgs>();
  int returnValue = returnAndArgs.Invoke(1, 2);
  std::cout << " " << returnValue << '\n'
            << "Open: " << basic.Open() << args.Open() << returnAndArgs.Open()
            << '\n';
}

void MemberFunction()
{
  struct Object
  {
    int mValue;
    Object(): mValue(0) {}
    void Basic()
    {
      ++mValue;
      std::cout << mValue << " Basic\n";
    }
    void Args(int i, float f)
    {
      ++mValue;
      std::cout << mValue << " Args " << i << " " << f << '\n';
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
  std::cout << " " << returnValue << '\n'
            << "Open: " << basic.Open() << args.Open() << returnAndArgs.Open()
            << '\n';
}

void NullFunction()
{
  Util::Delegate<void> test;
  test.BindNull();
  std::cout << "Open: " << test.Open() << '\n';
}

int main(void)
{
  RunTest(FreeFunction);
  RunTest(MemberFunction);
  RunTest(NullFunction);
}
