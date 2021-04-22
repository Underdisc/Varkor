#include "math/Constants.h"

#include "math/Utility.h"

namespace Math {

bool Near(float a, float b)
{
  float difference = a - b;
  return difference < nEpsilon && difference > -nEpsilon;
}

} // namespace Math
