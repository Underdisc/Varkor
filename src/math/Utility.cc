#include "math/Constants.h"

#include "math/Utility.h"

namespace Math {

bool Near(float a, float b)
{
  float difference = a - b;
  return difference < nEpsilon && difference > -nEpsilon;
}

float Sign(float value)
{
  if (value < 0) {
    return -1;
  }
  return 1;
}

float Round(float value)
{
  if (value < 0.0f) {
    return (float)(int)(value - 0.5f);
  }
  return (float)(int)(value + 0.5f);
}

float RoundToNearest(float value, float interval)
{
  float increments = value / interval;
  return interval * Round(increments);
}

} // namespace Math
