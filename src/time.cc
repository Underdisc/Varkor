#include <chrono>

#include "time.h"

namespace Time {

// todo: When working on creating linux builds, you will need to check the
// precision of the clock. The precision of steady_clock under msvc is 10^-7.
// Do not use std::chrono::high_resolution_clock. It is usually just an alias
// for steady_clock or system_clock.
typedef std::chrono::steady_clock Clock;

float nDeltaTime;
Clock::time_point nStart;
Clock::time_point nLastUpdate;

void Init()
{
  nStart = Clock::now();
  nLastUpdate = nStart;
}

void Update()
{
  Clock::time_point currentTime = Clock::now();
  std::chrono::nanoseconds dtNano = currentTime - nLastUpdate;
  std::chrono::duration<float> dt = dtNano;
  nDeltaTime = dt.count();
  nLastUpdate = currentTime;
}

float DeltaTime()
{
  return nDeltaTime;
}

} // namespace Time
