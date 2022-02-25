#include <cmath>

#include "math/Constants.h"
#include "math/Print.h"

std::ostream& operator<<(std::ostream& os, const Quaternion& q)
{
  bool firstValue = true;
  auto printValue = [&os, &firstValue](float value, const char* basis)
  {
    if (std::fabs(value) < Math::nEpsilon)
    {
      return;
    }
    if (firstValue)
    {
      os << value << basis;
      firstValue = false;
      return;
    }
    if (value < 0.0f)
    {
      os << " - ";
      value *= -1.0f;
    } else
    {
      os << " + ";
    }
    os << value << basis;
  };
  printValue(q.mA, "");
  printValue(q.mB, "i");
  printValue(q.mC, "j");
  printValue(q.mD, "k");
  return os;
}
