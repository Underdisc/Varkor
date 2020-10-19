#include <iostream>

#include "math/Complex.hh"
#include "math/Constants.h"

void Assignment()
{
  std::cout << "Assignment" << std::endl;
  Complex a = {1.0f, 2.0f};
  Complex b = a;
  std::cout << "a: " << a << std::endl;
  std::cout << "b: " << b << std::endl << std::endl;
}

void Addition()
{
  std::cout << "Addition" << std::endl;
  Complex a = {1.0f, 2.0f};
  Complex b = a;
  Complex c = a + b;
  b += a;
  std::cout << "a: " << a << std::endl;
  std::cout << "b: " << b << std::endl;
  std::cout << "c: " << c << std::endl << std::endl;
}

void Subtraction()
{
  std::cout << "Subtraction" << std::endl;
  Complex a = {1.0f, 2.0f};
  Complex b = {2.0f, 5.0f};
  Complex c = a - b;
  b -= a;
  std::cout << "a: " << a << std::endl;
  std::cout << "b: " << b << std::endl;
  std::cout << "c: " << c << std::endl << std::endl;
}

void Multiplication()
{
  std::cout << "Multiplication" << std::endl;
  Complex a = {1.0f, 2.0f};
  Complex b = {2.0f, 5.0f};
  Complex c = a * b;
  b *= a;
  Complex d = a * 3.0f;
  a *= 3.0f;
  std::cout << "a: " << a << std::endl;
  std::cout << "b: " << b << std::endl;
  std::cout << "c: " << c << std::endl;
  std::cout << "d: " << d << std::endl << std::endl;
}

void PolarComplex()
{
  Complex a = Math::ComplexPolar(2.0f, 0.0f);
  Complex b = Math::ComplexPolar(3.0f, PIO2f);
  Complex c = Math::ComplexPolar(1.0f, PIf);
  Complex d = Math::ComplexPolar(4.0f, PIf * 1.5f);
  Complex e = Math::ComplexPolar(1.0f, PIf / 4.0f);
  std::cout << "a: " << a << std::endl;
  std::cout << "b: " << b << std::endl;
  std::cout << "c: " << c << std::endl;
  std::cout << "d: " << d << std::endl;
  std::cout << "e: " << e << std::endl << std::endl;

}

int main(void)
{
  Assignment();
  Addition();
  Subtraction();
  Multiplication();
  PolarComplex();
}
