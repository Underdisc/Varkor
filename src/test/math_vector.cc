#include <iostream>

#include "../math/vector.hh"

void Cast()
{
  std::cout << "Cast" << std::endl;
  Math::Vector<float, 4> vec;
  vec[0] = 0;
  vec[1] = 1;
  vec[2] = 2;
  vec[3] = 3;

  Math::Vector<float, 2> smallerVec = (Math::Vector<float, 2>)vec;
  Math::Vector<float, 6> largerVec = (Math::Vector<float, 6>)vec;
  std::cout << "vec " << vec << std::endl;
  std::cout << "smallerVec " << smallerVec << std::endl;
  std::cout << "largerVec " << largerVec << std::endl << std::endl;
}

void Addition()
{
  std::cout << "Addition" << std::endl;
  Vec3 a = {1.0f, 2.0f, 3.0f};
  Vec3 b = a;
  Vec3 c = a + b;
  Vec3 d = {0.0f, 0.0f, 0.0f};
  d += c;
  std::cout << "a: " << a << std::endl;
  std::cout << "b: " << b << std::endl;
  std::cout << "c: " << c << std::endl;
  std::cout << "d: " << d << std::endl << std::endl;
}

void Subtraction()
{
  std::cout << "Subtraction" << std::endl;
  Vec3 a = {1.0f, 2.0f, 3.0f};
  Vec3 b = a;
  Vec3 c = a - b;
  Vec3 d = {0.0f, 0.0f, 0.0f};
  d -= a;
  std::cout << "a: " << a << std::endl;
  std::cout << "b: " << b << std::endl;
  std::cout << "c: " << c << std::endl;
  std::cout << "d: " << d << std::endl << std::endl;
}

void ScalerMultiplication()
{
  std::cout << "ScalerMultiplication" << std::endl;
  Vec3 a = {1.0f, 2.0f, 3.0f};
  Vec3 b = a * 2.0f;
  Vec3 c = 3.0f * a;
  Vec3 d = a;
  d *= 4.0f;
  std::cout << "a: " << a << std::endl;
  std::cout << "b: " << b << std::endl;
  std::cout << "c: " << c << std::endl;
  std::cout << "d: " << d << std::endl << std::endl;
}

void ScalerDivision()
{
  std::cout << "ScalerDivision" << std::endl;
  Vec3 a = {2.0f, 4.0f, 6.0f};
  Vec3 b = a / 2.0f;
  Vec3 c = a;
  c /= 2.0f;
  std::cout << "a: " << a << std::endl;
  std::cout << "b: " << b << std::endl;
  std::cout << "c: " << c << std::endl << std::endl;
}

void Negation()
{
  std::cout << "Negation" << std::endl;
  Vec3 a = {2.0f, -4.0f, 6.0f};
  Vec3 b = -a;
  std::cout << "a: " << a << std::endl;
  std::cout << "b: " << b << std::endl << std::endl;
}

void MagnitudeSquared()
{
  std::cout << "MagnitudeSquared" << std::endl;
  Vec3 a = {1.0f, -2.0f, 3.0f};
  float magSq = Math::MagnitudeSq(a);
  std::cout << "a: " << a << std::endl;
  std::cout << "magnitude squared: " << magSq << std::endl << std::endl;
}

void Magnitude()
{
  std::cout << "Magnitude" << std::endl;
  Vec3 a = {1.0f, 2.0f, 3.0f};
  float mag = Math::Magnitude(a);
  std::cout << "a: " << a << std::endl;
  std::cout << "magnitude: " << mag << std::endl << std::endl;
}

void Normalize()
{
  std::cout << "Normalize" << std::endl;
  Vec3 a = {1.0f, 2.0f, 3.0f};
  std::cout << "original: " << a << std::endl;
  a = Math::Normalize(a);
  std::cout << "normalized: " << a << std::endl << std::endl;
}

void Cross()
{
  std::cout << "Cross" << std::endl;
  Vec3 a = {1.0f, 2.0f, 3.0f};
  Vec3 b = {4.0f, 5.0f, 6.0f};
  Vec3 cross = Math::Cross(a, b);
  std::cout << "a: " << a << std::endl;
  std::cout << "b: " << b << std::endl;
  std::cout << "cross: " << cross << std::endl << std::endl;
}

void Dot()
{
  std::cout << "Dot" << std::endl;
  Vec3 a = {1.0f, 2.0f, 3.0f};
  Vec3 b = {4.0f, 5.0f, 6.0f};
  float dot = Math::Dot(a, b);
  std::cout << "a: " << a << std::endl;
  std::cout << "b: " << b << std::endl;
  std::cout << "dot: " << dot << std::endl << std::endl;
}

void NormalizeZeroVector()
{
  std::cout << "NormalizeZeroVector" << std::endl;
  Vec3 a = {0.0f, 0.0f, 0.0f};
  Math::Normalize(a);
}

int main(void)
{
  Cast();
  Addition();
  Subtraction();
  ScalerMultiplication();
  ScalerDivision();
  Negation();
  MagnitudeSquared();
  Magnitude();
  Normalize();
  Cross();
  Dot();

  // This function will cause an abort. Normalizing a zero vector means there
  // will be a division by zero. The function aborts before this happens.
  /*
  NormalizeZeroVector();
  */
}