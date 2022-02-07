#include <iostream>
#include <math.h>

#include "math/Constants.h"
#include "math/Quaternion.h"

std::ostream& operator<<(std::ostream& os, const Quat& quat)
{
  os << quat.mA << " + i(" << quat.mB << ") + j(" << quat.mC << ") + k("
     << quat.mD << ")";
  return os;
}

void AngleAxis()
{
  std::cout << "<= AngleAxis =>" << std::endl;
  float angle = Math::nPiO2;
  Vec3 axis = {1.0f, 1.0f, 1.0f};
  axis = Math::Normalize(axis);
  Quat quat;
  quat.AngleAxis(angle, axis);
  std::cout << "angle: " << angle << std::endl;
  std::cout << "axis: " << axis << std::endl;
  std::cout << "quaternion: " << quat << std::endl << std::endl;
}

void FromTo()
{
  std::cout << "<= FromTo =>" << std::endl;
  Vec3 v[4];
  Math::Quaternion q[3];
  v[0] = {1.0f, 0.0f, 0.0f};
  v[1] = {0.0f, 1.0f, 0.0f};
  v[2] = {0.0f, 0.0f, 1.0f};
  v[3] = {-1.0f, 0.0f, 0.0f};
  q[0].FromTo(v[0], v[1]);
  q[1].FromTo(v[1], v[2]);
  q[2].FromTo(v[0], v[3]);
  std::cout << q[0] << std::endl;
  std::cout << q[0].Rotate(v[0]) << std::endl;
  std::cout << q[1] << std::endl;
  std::cout << q[1].Rotate(v[1]) << std::endl;
  std::cout << q[2] << std::endl;
  std::cout << q[2].Rotate(v[0]) << std::endl << std::endl;
}

void Interpolate()
{
  std::cout << "<= Interpolate =>\n";
  std::cout << "-NegativeZAxis-\n";
  Math::Quaternion q;
  q.AngleAxis(Math::nPi, {0.0f, 0.0f, -1.0f});
  std::cout << q.Interpolate(0.25f) << '\n';
  std::cout << q.Interpolate(0.5f) << '\n';
  std::cout << q.Interpolate(0.75f) << '\n';
  std::cout << q.Interpolate(1.0f) << '\n';
  std::cout << "-PositiveAxis-\n";
  q.AngleAxis(Math::nPi, {1.0f, 1.0f, 1.0f});
  std::cout << q.Interpolate(0.25f) << '\n';
  std::cout << q.Interpolate(0.5f) << '\n';
  std::cout << q.Interpolate(0.75f) << '\n';
  std::cout << q.Interpolate(1.0f) << '\n';
  std::cout << q << "\n\n";
}

void Conjugate()
{
  std::cout << "<= Conjugate =>" << std::endl;
  Quat quat = {1.0f, 2.0f, 3.0f, 4.0f};
  Quat conj = quat.Conjugate();
  std::cout << "quaternion: " << quat << std::endl;
  std::cout << "conjugate: " << conj << std::endl << std::endl;
}

void Multiplication()
{
  std::cout << "<= Multiplicaton =>" << std::endl;
  Quat quat1 = {1.0f, 2.0f, 3.0f, 4.0f};
  Quat quat2 = {4.0f, 3.0f, 2.0f, 1.0f};
  Quat result = quat1 * quat2;
  std::cout << "quaternion 1: " << quat1 << std::endl;
  std::cout << "quaternion 2: " << quat1 << std::endl;
  std::cout << "result: " << result << std::endl << std::endl;
}

void EulerAngles()
{
  std::cout << "<= EulerAngles =>" << std::endl;
  Quat q;
  q.AngleAxis(Math::nPi / 2.0f, {1.0f, 0.0f, 0.0f});
  Vec3 eulerAngles = q.EulerAngles();
  std::cout << eulerAngles << std::endl;
  q.AngleAxis(Math::nPi / 2.0f, {0.0f, 1.0f, 0.0f});
  eulerAngles = q.EulerAngles();
  std::cout << eulerAngles << std::endl;
  q.AngleAxis(Math::nPi / 2.0f, {0.0f, 0.0f, 1.0f});
  eulerAngles = q.EulerAngles();
  std::cout << eulerAngles << std::endl << std::endl;
}

void Rotate()
{
  std::cout << "<= Rotate =>" << std::endl;
  Math::Quaternion a, b;
  a = {0.0f, 1.0f / std::sqrtf(2.0f), 0.0f, 1.0f / std::sqrtf(2.0f)};
  b.AngleAxis(Math::nPi / 2.0f, {0.0f, 0.0f, 1.0f});
  std::cout << a.Rotate({1.0f, 0.0f, 0.0f}) << std::endl;
  std::cout << a.Rotate({-1.0f, 0.0f, -1.0f}) << std::endl;
  std::cout << b.Rotate({1.0f, 0.0f, 0.0f}) << std::endl;
  std::cout << b.Rotate({0.0f, 1.0f, 0.0f}) << std::endl;
}

int main(void)
{
  AngleAxis();
  FromTo();
  Interpolate();
  Conjugate();
  Multiplication();
  EulerAngles();
  Rotate();
}
