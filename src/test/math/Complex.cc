#include <iostream>

#include "math/Complex.h"
#include "math/Constants.h"
#include "test/Test.h"

void Assignment()
{
  Complex a = {1.0f, 2.0f};
  Complex b = a;
  std::cout << "a: " << a << '\n';
  std::cout << "b: " << b << '\n';
}

void Addition()
{
  Complex a = {1.0f, 2.0f};
  Complex b = a;
  Complex c = a + b;
  b += a;
  std::cout << "a: " << a << '\n';
  std::cout << "b: " << b << '\n';
  std::cout << "c: " << c << '\n';
}

void Subtraction()
{
  Complex a = {1.0f, 2.0f};
  Complex b = {2.0f, 5.0f};
  Complex c = a - b;
  b -= a;
  std::cout << "a: " << a << '\n';
  std::cout << "b: " << b << '\n';
  std::cout << "c: " << c << '\n';
}

void Multiplication()
{
  Complex a = {1.0f, 2.0f};
  Complex b = {2.0f, 5.0f};
  Complex c = a * b;
  b *= a;
  Complex d = a * 3.0f;
  a *= 3.0f;
  std::cout << "a: " << a << '\n';
  std::cout << "b: " << b << '\n';
  std::cout << "c: " << c << '\n';
  std::cout << "d: " << d << '\n';
}

void PolarComplex()
{
  Complex a = Math::ComplexPolar(2.0f, 0.0f);
  Complex b = Math::ComplexPolar(3.0f, Math::nPiO2);
  Complex c = Math::ComplexPolar(1.0f, Math::nPi);
  Complex d = Math::ComplexPolar(4.0f, Math::nPi * 1.5f);
  Complex e = Math::ComplexPolar(1.0f, Math::nPi / 4.0f);
  std::cout << "a: " << a << '\n';
  std::cout << "b: " << b << '\n';
  std::cout << "c: " << c << '\n';
  std::cout << "d: " << d << '\n';
  std::cout << "e: " << e << '\n';
}

int main(void)
{
  RunTest(Assignment);
  RunTest(Addition);
  RunTest(Subtraction);
  RunTest(Multiplication);
  RunTest(PolarComplex);
}
