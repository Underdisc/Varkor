#include <iostream>

#include "../math/vector.hh"

void Cast()
{
  Math::Vector<float, 4> vec;
  vec[0] = 0;
  vec[1] = 1;
  vec[2] = 2;
  vec[3] = 3;

  Math::Vector<float, 2> smallerVec = (Math::Vector<float, 2>)vec;
  Math::Vector<float, 6> largerVec = (Math::Vector<float, 6>)vec;
  std::cout << "vec " << vec << std::endl;
  std::cout << "smallerVec " << smallerVec << std::endl;
  std::cout << "largerVec " << largerVec << std::endl;
}

void Cross()
{
  Vec3 a = {1.0f, 2.0f, 3.0f};
  Vec3 b = {4.0f, 5.0f, 6.0f};
  Vec3 cross = Math::Cross(a, b);
  std::cout << "a: " << a << std::endl;
  std::cout << "b: " << b << std::endl;
  std::cout << "cross: " << cross << std::endl;
}

void Dot()
{
  Vec3 a = {1.0f, 2.0f, 3.0f};
  Vec3 b = {4.0f, 5.0f, 6.0f};
  float dot = Math::Dot(a, b);
  std::cout << "a: " << a << std::endl;
  std::cout << "b: " << b << std::endl;
  std::cout << "dot: " << dot << std::endl;
}

void Normalize()
{
  Vec3 a = {1.0f, 2.0f, 3.0f};
  std::cout << "original: " << a << std::endl;
  a = Math::Normalize(a);
  std::cout << "normalized: " << a << std::endl;
}

int main(void)
{
  // Cast();
  // Cross();
  // Dot();
  // Normalize();
}
