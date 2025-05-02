#include <iostream>
#include <math.h>

#include "math/Constants.h"
#include "math/Print.h"
#include "math/Quaternion.h"
#include "test/Test.h"

void AngleAxis() {
  float angle = Math::nPiO2;
  Vec3 axis = {1.0f, 1.0f, 1.0f};
  axis = Math::Normalize(axis);
  Quat quat = Quat::AngleAxis(angle, axis);
  std::cout << "angle: " << angle << '\n';
  std::cout << "axis: " << axis << '\n';
  std::cout << "quaternion: " << quat << '\n';
}

void FromTo() {
  Vec3 v[4];
  Math::Quaternion q[3];
  v[0] = {1.0f, 0.0f, 0.0f};
  v[1] = {0.0f, 1.0f, 0.0f};
  v[2] = {0.0f, 0.0f, 1.0f};
  v[3] = {-1.0f, 0.0f, 0.0f};
  q[0] = Quat::FromTo(v[0], v[1]);
  q[1] = Quat::FromTo(v[1], v[2]);
  q[2] = Quat::FromTo(v[0], v[3]);
  std::cout << q[0] << '\n';
  std::cout << q[0].Rotate(v[0]) << '\n';
  std::cout << q[1] << '\n';
  std::cout << q[1].Rotate(v[1]) << '\n';
  std::cout << q[2] << '\n';
  std::cout << q[2].Rotate(v[0]) << '\n';
}

void BasisVectors() {
  auto testBasis = [](Vec3 xAxis, Vec3 up) {
    xAxis = Math::Normalize(xAxis);
    Vec3 zAxis = Math::Normalize(Math::Cross(xAxis, up));
    Vec3 yAxis = Math::Normalize(Math::Cross(zAxis, xAxis));

    Quat q = Quat::BasisVectors(xAxis, yAxis, zAxis);
    std::cout << q << '\n';
  };
  testBasis({1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
  testBasis({1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f});
  testBasis({-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
  testBasis({-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f});
  testBasis({1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f});
  testBasis({1.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f});
  testBasis({-1.0f, -1.0f, -1.0f}, {0.0f, 1.0f, 0.0f});
  testBasis({-1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f});
  testBasis({-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f});
  testBasis({-1.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f});
  testBasis({1.0f, -1.0f, -1.0f}, {0.0f, 1.0f, 0.0f});
  testBasis({1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f});
}

void Interpolate() {
  std::cout << "-NegativeZAxis-\n";
  Quat q = Quat::AngleAxis(Math::nPi, {0.0f, 0.0f, -1.0f});
  std::cout << q.Interpolate(0.25f) << '\n';
  std::cout << q.Interpolate(0.5f) << '\n';
  std::cout << q.Interpolate(0.75f) << '\n';
  std::cout << q.Interpolate(1.0f) << '\n';
  std::cout << "-PositiveAxis-\n";
  q = Quat::AngleAxis(Math::nPi, {1.0f, 1.0f, 1.0f});
  std::cout << q.Interpolate(0.25f) << '\n';
  std::cout << q.Interpolate(0.5f) << '\n';
  std::cout << q.Interpolate(0.75f) << '\n';
  std::cout << q.Interpolate(1.0f) << '\n';
  std::cout << q << "\n";
}

void Conjugate() {
  Quat quat = {1.0f, 2.0f, 3.0f, 4.0f};
  Quat conj = quat.Conjugate();
  std::cout << "quaternion: " << quat << '\n';
  std::cout << "conjugate: " << conj << '\n';
}

void Multiplication() {
  Quat quat1 = {1.0f, 2.0f, 3.0f, 4.0f};
  Quat quat2 = {4.0f, 3.0f, 2.0f, 1.0f};
  Quat result = quat1 * quat2;
  std::cout << "quaternion 1: " << quat1 << '\n';
  std::cout << "quaternion 2: " << quat1 << '\n';
  std::cout << "result: " << result << '\n';
}

void EulerAngles() {
  Quat q = Quat::AngleAxis(Math::nPi / 2.0f, {1.0f, 0.0f, 0.0f});
  Vec3 eulerAngles = q.EulerAngles();
  std::cout << eulerAngles << '\n';
  q = Quat::AngleAxis(Math::nPi / 2.0f, {0.0f, 1.0f, 0.0f});
  eulerAngles = q.EulerAngles();
  std::cout << eulerAngles << '\n';
  q = Quat::AngleAxis(Math::nPi / 2.0f, {0.0f, 0.0f, 1.0f});
  eulerAngles = q.EulerAngles();
  std::cout << eulerAngles << '\n';
}

void Rotate() {
  Quat a, b;
  a = {0.0f, 1.0f / sqrtf(2.0f), 0.0f, 1.0f / sqrtf(2.0f)};
  b = Quat::AngleAxis(Math::nPi / 2.0f, {0.0f, 0.0f, 1.0f});
  std::cout << a.Rotate({1.0f, 0.0f, 0.0f}) << '\n';
  std::cout << a.Rotate({-1.0f, 0.0f, -1.0f}) << '\n';
  std::cout << b.Rotate({1.0f, 0.0f, 0.0f}) << '\n';
  std::cout << b.Rotate({0.0f, 1.0f, 0.0f}) << '\n';
}

int main(void) {
  RunTest(AngleAxis);
  RunTest(FromTo);
  RunTest(BasisVectors);
  RunTest(Interpolate);
  RunTest(Conjugate);
  RunTest(Multiplication);
  RunTest(EulerAngles);
  RunTest(Rotate);
}
