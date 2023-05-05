#include <string>

#include "ds/Pool.h"
#include "test/Test.h"
#include "test/ds/Print.h"

Ds::Pool<std::string> AlphabetSoup(int count)
{
  Ds::Pool<std::string> pool;
  for (int i = 0; i < count; ++i) {
    std::stringstream ss;
    ss << (char)('a' + i);
    pool.Add(ss.str());
  }
  return pool;
}

void Add()
{
  Ds::Pool<std::string> test = AlphabetSoup(10);
  PrintPool(test);
}

void Remove0()
{
  Ds::Pool<std::string> test = AlphabetSoup(1);
  test.Remove(0);
  PrintPool(test);
}

void Remove1()
{
  Ds::Pool<std::string> test = AlphabetSoup(2);
  test.Remove(0);
  PrintPool(test);
}

void Remove2()
{
  Ds::Pool<std::string> test = AlphabetSoup(4);
  test.Remove(2);
  test.Remove(1);
  test.Remove(3);
  PrintPool(test);
}

void Remove3()
{
  Ds::Pool<std::string> test = AlphabetSoup(10);
  for (int i = 8; i >= 0; i -= 2) {
    test.Remove(i);
  }
  PrintPool(test);
}

void Remove4()
{
  Ds::Pool<std::string> test = AlphabetSoup(10);
  for (int i = 9; i > 0; --i) {
    test.Remove(i);
  }
  PrintPool(test);
}

void RemoveAdd0()
{
  Ds::Pool<std::string> test = AlphabetSoup(4);
  test.Remove(3);
  test.Add("e");
  PrintPool(test);
}

void RemoveAdd1()
{
  Ds::Pool<std::string> test = AlphabetSoup(4);
  test.Remove(2);
  test.Remove(3);
  test.Add("e");
  test.Add("f");
  PrintPool(test);
}

void RemoveAdd2()
{
  Ds::Pool<std::string> test = AlphabetSoup(5);
  test.Remove(1);
  test.Remove(0);
  test.Add("f");
  test.Add("g");
  test.Add("h");
  PrintPool(test);
}

void IndexOperator()
{
  Ds::Pool<std::string> test = AlphabetSoup(3);
  test.Remove(0);
  test.Remove(2);
  test.Remove(1);
  Ds::PoolId ids[3] = {test.Add("a"), test.Add("b"), test.Add("c")};
  std::cout << test[ids[0]] << ", " << test[ids[1]] << ", " << test[ids[2]]
            << '\n';
  PrintPool(test);
}

int main()
{
  EnableLeakOutput();
  RunTest(Add);
  RunTest(Remove0);
  RunTest(Remove1);
  RunTest(Remove2);
  RunTest(Remove3);
  RunTest(Remove4);
  RunTest(RemoveAdd0);
  RunTest(RemoveAdd1);
  RunTest(RemoveAdd2);
  RunTest(IndexOperator);
}