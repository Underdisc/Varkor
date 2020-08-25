#include <iostream>

#include "../src/math/vector.hh"

void test_cast()
{
  Vector<float, 4> vec;
  vec[0] = 0;
  vec[1] = 1;
  vec[2] = 2;
  vec[3] = 3;

  Vector<float, 2> smaller_vec = (Vector<float, 2>)vec;
  Vector<float, 6> larger_vec = (Vector<float, 6>)vec;
  std::cout << "vec " << vec << std::endl;
  std::cout << "smaller_vec " << smaller_vec << std::endl;
  std::cout << "larger_vec " << larger_vec << std::endl;
}

int main(void)
{
  test_cast();
}
