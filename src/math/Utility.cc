#include "math/Constants.h"

#include "math/Utility.h"

namespace Math {

bool Near(float a, float b)
{
  float difference = a - b;
  return difference < nEpsilon && difference > -nEpsilon;
}

float Round(float value)
{
  return (float)(int)(value + 0.5f);
}

float RoundToNearest(float value, float interval)
{
  float increments = value / interval;
  return interval * Round(increments);
}

} // namespace Math
