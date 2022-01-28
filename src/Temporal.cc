#include "Framer.h"

#include "Temporal.h"

namespace Temporal {

float nTotalTime = 0.0f;

void Update()
{
  nTotalTime += DeltaTime();
}

float DeltaTime()
{
  return Framer::FrameTime();
}

float TotalTime()
{
  return nTotalTime;
}

} // namespace Temporal
