#include <string>

#include "ds/Pool.h"
#include "test/Test.h"
#include "test/ds/Print.h"

Ds::Pool<std::string> AlphabetSoup(int count) {
  Ds::Pool<std::string> pool;
  for (int i = 0; i < count; ++i) {
    std::stringstream ss;
    ss << (char)('a' + i);
    pool.Add(ss.str());
  }
  return pool;
}

void Add() {
  Ds::Pool<std::string> test = AlphabetSoup(10);
  PrintPool(test);
}

void Copy() {
  Ds::Pool<std::string> test = AlphabetSoup(10);
  Ds::Pool<std::string> copied(test);
  PrintPool(test);
  PrintPool(copied);
}

void Move() {
  Ds::Pool<std::string> test = AlphabetSoup(10);
  Ds::Pool<std::string> moved(std::move(test));
  PrintPool(test);
  PrintPool(moved);
}

void Request0() {
  Ds::Pool<std::string> test;
  test.Request(5) = 'a';
  PrintPool(test);
}

void Request1() {
  Ds::Pool<std::string> test = AlphabetSoup(5);
  test.Request(10) = 'a' + 9;
  PrintPool(test);
}

void Request2() {
  Ds::Pool<std::string> test;
  test.Request(4) = 'e';
  test.Request(0) = 'a';
  test.Request(1) = 'b';
  PrintPool(test);
}

void Remove0() {
  Ds::Pool<std::string> test = AlphabetSoup(1);
  test.Remove(0);
  PrintPool(test);
}

void Remove1() {
  Ds::Pool<std::string> test = AlphabetSoup(2);
  test.Remove(0);
  PrintPool(test);
}

void Remove2() {
  Ds::Pool<std::string> test = AlphabetSoup(4);
  test.Remove(2);
  test.Remove(1);
  test.Remove(3);
  PrintPool(test);
}

void Remove3() {
  Ds::Pool<std::string> test = AlphabetSoup(10);
  for (int i = 8; i >= 0; i -= 2) {
    test.Remove(i);
  }
  PrintPool(test);
}

void Remove4() {
  Ds::Pool<std::string> test = AlphabetSoup(10);
  for (int i = 9; i > 0; --i) {
    test.Remove(i);
  }
  PrintPool(test);
}

void Remove5() {
  Ds::Pool<std::string> test;
  test.Request(4) = 'e';
  test.Request(0) = 'a';
  test.Request(1) = 'b';
  test.Remove(4);
  PrintPool(test);
}

void RemoveAdd0() {
  Ds::Pool<std::string> test = AlphabetSoup(4);
  test.Remove(3);
  test.Add("e");
  PrintPool(test);
}

void RemoveAdd1() {
  Ds::Pool<std::string> test = AlphabetSoup(4);
  test.Remove(2);
  test.Remove(3);
  test.Add("e");
  test.Add("f");
  PrintPool(test);
}

void RemoveAdd2() {
  Ds::Pool<std::string> test = AlphabetSoup(5);
  test.Remove(1);
  test.Remove(0);
  test.Add("f");
  test.Add("g");
  test.Add("h");
  PrintPool(test);
}

void RemoveRequest0() {
  Ds::Pool<std::string> test = AlphabetSoup(2);
  test.Remove(0);
  test.Request(3) = "c";
  PrintPool(test);
}

void RemoveRequestAdd0() {
  Ds::Pool<std::string> test = AlphabetSoup(2);
  test.Remove(0);
  test.Request(3) = "c";
  test.Add("d");
  test.Add("e");
  PrintPool(test);
}

void Clear() {
  Ds::Pool<std::string> test = AlphabetSoup(5);
  test.Clear();
  PrintPool(test);
}

void IndexOperator() {
  Ds::Pool<std::string> test = AlphabetSoup(3);
  test.Remove(0);
  test.Remove(2);
  test.Remove(1);
  Ds::PoolId ids[3];
  ids[0] = test.Add("a");
  ids[1] = test.Add("b");
  ids[2] = test.Add("c");
  std::cout << test[ids[0]] << ", " << test[ids[1]] << ", " << test[ids[2]]
            << '\n';
  PrintPool(test);
}

int main() {
  Error::Init();
  EnableLeakOutput();
  RunTest(Add);
  RunTest(Copy);
  RunTest(Move);
  RunTest(Request0);
  RunTest(Request1);
  RunTest(Request2);
  RunTest(Remove0);
  RunTest(Remove1);
  RunTest(Remove2);
  RunTest(Remove3);
  RunTest(Remove4);
  RunTest(Remove5);
  RunTest(RemoveAdd0);
  RunTest(RemoveAdd1);
  RunTest(RemoveAdd2);
  RunTest(RemoveRequest0);
  RunTest(RemoveRequestAdd0);
  RunTest(Clear);
  RunTest(IndexOperator);
}
