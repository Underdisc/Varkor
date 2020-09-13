#include <iostream>

#include "../math/complex.hh"

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

int main(void)
{
  // Assignment();
  // Addition();
  // Subtraction();
  // Multiplication();
}
