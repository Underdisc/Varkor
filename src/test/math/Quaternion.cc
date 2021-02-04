#include <iostream>
#include <math.h>

#include "math/Constants.h"
#include "math/Quaternion.h"

void AngleAxis()
{
  std::cout << "AngleAxis" << std::endl;
  float angle = Math::nPiO2;
  Vec3 axis = {1.0f, 1.0f, 1.0f};
  axis = Math::Normalize(axis);
  Quat quat;
  quat.AngleAxis(angle, axis);
  std::cout << "angle: " << angle << std::endl;
  std::cout << "axis: " << axis << std::endl;
  std::cout << "quaternion: " << quat << std::endl << std::endl;
}

void Conjugate()
{
  std::cout << "Conjugate" << std::endl;
  Quat quat = {1.0f, 2.0f, 3.0f, 4.0f};
  Quat conj = quat.Conjugate();
  std::cout << "quaternion: " << quat << std::endl;
  std::cout << "conjugate: " << conj << std::endl << std::endl;
}

void Multiplication()
{
  std::cout << "Multiplicaton" << std::endl;
  Quat quat1 = {1.0f, 2.0f, 3.0f, 4.0f};
  Quat quat2 = {4.0f, 3.0f, 2.0f, 1.0f};
  Quat result = quat1 * quat2;
  std::cout << "quaternion 1: " << quat1 << std::endl;
  std::cout << "quaternion 2: " << quat1 << std::endl;
  std::cout << "result: " << result << std::endl << std::endl;
}

void Test()
{
  Quat q = {0.0f, 1.0f / std::sqrtf(2.0f), 0.0f, 1.0f / std::sqrtf(2.0f)};
  Quat p = {0.0f, 1.0f, 0.0f, 0.0f};
  std::cout << "q: " << q << std::endl;
  std::cout << "p: " << p << std::endl;
  p = q * p * q.Conjugate();
  std::cout << "q * p * q': " << p << std::endl << std::endl;
}

int main(void)
{
  AngleAxis();
  Conjugate();
  Multiplication();
  Test();
}
