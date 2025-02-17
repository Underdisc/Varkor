#include <iostream>

#include "math/Constants.h"
#include "math/Vector.h"
#include "test/Test.h"

void Cast()
{
  Math::Vector<float, 4> vec;
  vec[0] = 0;
  vec[1] = 1;
  vec[2] = 2;
  vec[3] = 3;

  Math::Vector<float, 2> smallerVec = (Math::Vector<float, 2>)vec;
  Math::Vector<float, 6> largerVec = (Math::Vector<float, 6>)vec;
  std::cout << "vec " << vec << '\n';
  std::cout << "smallerVec " << smallerVec << '\n';
  std::cout << "largerVec " << largerVec << '\n';
}

void Addition()
{
  Vec3 a = {1.0f, 2.0f, 3.0f};
  Vec3 b = a;
  Vec3 c = a + b;
  Vec3 d = {0.0f, 0.0f, 0.0f};
  d += c;
  std::cout << "a: " << a << '\n';
  std::cout << "b: " << b << '\n';
  std::cout << "c: " << c << '\n';
  std::cout << "d: " << d << '\n';
}

void Subtraction()
{
  Vec3 a = {1.0f, 2.0f, 3.0f};
  Vec3 b = a;
  Vec3 c = a - b;
  Vec3 d = {0.0f, 0.0f, 0.0f};
  d -= a;
  std::cout << "a: " << a << '\n';
  std::cout << "b: " << b << '\n';
  std::cout << "c: " << c << '\n';
  std::cout << "d: " << d << '\n';
}

void ScalerMultiplication()
{
  Vec3 a = {1.0f, 2.0f, 3.0f};
  Vec3 b = a * 2.0f;
  Vec3 c = 3.0f * a;
  Vec3 d = a;
  d *= 4.0f;
  std::cout << "a: " << a << '\n';
  std::cout << "b: " << b << '\n';
  std::cout << "c: " << c << '\n';
  std::cout << "d: " << d << '\n';
}

void ScalerDivision()
{
  Vec3 a = {2.0f, 4.0f, 6.0f};
  Vec3 b = a / 2.0f;
  Vec3 c = a;
  c /= 2.0f;
  Vec3 d = 12.0f / a;
  std::cout << "a: " << a << '\n';
  std::cout << "b: " << b << '\n';
  std::cout << "c: " << c << '\n';
  std::cout << "d: " << d << '\n';
}

void Negation()
{
  Vec3 a = {2.0f, -4.0f, 6.0f};
  Vec3 b = -a;
  std::cout << "a: " << a << '\n';
  std::cout << "b: " << b << '\n';
}

void Equality()
{
  Vec3 a = {1.0f, 0.0f, 0.0f};
  Vec3 b = {2.0f, 0.0f, 1.0f};
  Vec3 c = {2.0f, 0.0f, 1.0f};
  std::cout << (a == a) << (a == b) << (b == c) << '\n';
}

void Near()
{
  Vec3 a = {1.0f, 0.0f, 0.0f};
  Vec3 b = {0.0f, 1.0f, 0.0f};
  Vec3 c = {Math::nEpsilon * Math::nEpsilon, 1.0f, 0.0f};
  std::cout << Math::Near(a, b) << Math::Near(a, c) << Math::Near(b, c) << '\n';
}

void MagnitudeSquared()
{
  Vec3 a = {1.0f, -2.0f, 3.0f};
  float magSq = Math::MagnitudeSq(a);
  std::cout << "a: " << a << '\n';
  std::cout << "magnitude squared: " << magSq << '\n';
}

void Magnitude()
{
  Vec3 a = {1.0f, 2.0f, 3.0f};
  float mag = Math::Magnitude(a);
  std::cout << "a: " << a << '\n';
  std::cout << "magnitude: " << mag << '\n';
}

void Normalize()
{
  Vec3 a = {1.0f, 2.0f, 3.0f};
  std::cout << "original: " << a << '\n';
  a = Math::Normalize(a);
  std::cout << "normalized: " << a << '\n';

  // This should cause an abort because the vector has a magnitude of zero.
  // a = {0.0f, 0.0f, 0.0f};
  // Math::Normalize(a);
}

void Cross()
{
  Vec3 a = {1.0f, 2.0f, 3.0f};
  Vec3 b = {4.0f, 5.0f, 6.0f};
  Vec3 cross = Math::Cross(a, b);
  std::cout << "a: " << a << '\n';
  std::cout << "b: " << b << '\n';
  std::cout << "cross: " << cross << '\n';
}

void Dot()
{
  Vec3 a = {1.0f, 2.0f, 3.0f};
  Vec3 b = {4.0f, 5.0f, 6.0f};
  float dot = Math::Dot(a, b);
  std::cout << "a: " << a << '\n';
  std::cout << "b: " << b << '\n';
  std::cout << "dot: " << dot << '\n';
}

void ComponentwiseProduct()
{
  Vec3 a = {1.0f, 2.0f, 3.0f};
  Vec3 b = {3.0f, 2.0f, 1.0f};
  std::cout << Math::ComponentwiseProduct(a, a) << '\n';
  std::cout << Math::ComponentwiseProduct(b, b) << '\n';
  std::cout << Math::ComponentwiseProduct(a, b) << '\n';
}

void PerpendicularTo()
{
  Vec3 a = {0.0f, 0.5f, 0.0f};
  Vec3 b = {1.0f, 1.0f, -1.0f};
  Vec3 c = {1.0f, 0.0f, -2.0f};
  std::cout << Math::PerpendicularTo(a) << '\n';
  std::cout << Math::PerpendicularTo(b) << '\n';
  std::cout << Math::PerpendicularTo(c) << '\n';
}

void ScaleToInterval()
{
  std::cout << Math::ScaleToInterval(Vec3({1.0f, 0.0f, 0.0f}), 0.5f) << '\n'
            << Math::ScaleToInterval(Vec3({0.2f, 0.0f, 0.0f}), 0.5f) << '\n'
            << Math::ScaleToInterval(Vec3({0.0f, 0.6f, 0.6f}), 1.0f) << '\n'
            << Math::ScaleToInterval(Vec3({1.0f, 1.0f, 1.0f}), 2.5f) << '\n'
            << Math::ScaleToInterval(Vec3({-1.0f, -1.0f, -1.0f}), 0.5f) << '\n';
}

void ScaleComponentsToInterval()
{
  // clang-format off
  std::cout
    << Math::ScaleComponentsToInterval(Vec3({1.0f, 0.5f, 6.6f}), 0.5f) << '\n'
    << Math::ScaleComponentsToInterval(Vec3({0.2f, 0.3f, 0.4f}), 0.5f) << '\n'
    << Math::ScaleComponentsToInterval(Vec3({-0.8f, 0.4f, 0.6f}), 1.0f) << '\n'
    << Math::ScaleComponentsToInterval(Vec3({1.0f, 4.9f, -14.4f}), 2.5f) << '\n'
    << Math::ScaleComponentsToInterval(Vec3({-1.4f, -1.2f, 6.1f}), 0.5f)
    << '\n';
  // clang-format on
}

int main(void)
{
  RunTest(Cast);
  RunTest(Addition);
  RunTest(Subtraction);
  RunTest(ScalerMultiplication);
  RunTest(ScalerDivision);
  RunTest(Negation);
  RunTest(Equality);
  RunTest(Near);
  RunTest(MagnitudeSquared);
  RunTest(Magnitude);
  RunTest(Normalize);
  RunTest(Cross);
  RunTest(Dot);
  RunTest(ComponentwiseProduct);
  RunTest(PerpendicularTo);
  RunTest(ScaleToInterval);
  RunTest(ScaleComponentsToInterval);
}
