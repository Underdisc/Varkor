#include <GLFW/glfw3.h>
#include <chrono>
#include <thread>

#include "Framer.h"

namespace Framer {

// todo: When working on creating linux builds, you will need to check the
// precision of the clock. The precision of steady_clock under msvc 1927 is
// 10^-7. Do not use std::chrono::high_resolution_clock. It is usually just an
// alias for steady_clock or system_clock.
typedef std::chrono::steady_clock Clock;
typedef std::chrono::nanoseconds Nanoseconds;
typedef std::chrono::milliseconds Milliseconds;

Nanoseconds nTargetFrameTimeNano;
int nFramerate = 0;
Clock::time_point nStartTime;
float nDeltaTime = 0.0f;

// A framerate of 0 will result in an uncapped framerate. Any value above zero
// will treat that value as the target framerate. Frame limiting is done by
// sleeping the thread.
void SetFramerate(int framerate)
{
  glfwSwapInterval(0);
  if (framerate == 0)
  {
    nFramerate = 0;
    nTargetFrameTimeNano = Nanoseconds(0);
    return;
  }
  nFramerate = framerate;
  nTargetFrameTimeNano = Nanoseconds(Nanoseconds::period::den / framerate);
}

// With this, frame limiting is handled by glfw and the gpu's refresh rate.
void VSyncFrameLimiting()
{
  glfwSwapInterval(1);
  nFramerate = 0;
  nTargetFrameTimeNano = Nanoseconds(0);
}

void Start()
{
  nStartTime = Clock::now();
}

// sleep_for has millisecond precision. This lack of precision will result
// in framerates that are slightly off of the desired framerate. For example, 60
// fps will technically be 59 fps, because sleep_for will often cause a delay
// that results in a 17ms frame time. This frame time is closer to the frame
// time of 59 fps than it is to 60. This is the behaviour under msvc 1927.
void End()
{
  Clock::time_point endTime = Clock::now();
  if (nFramerate != 0)
  {
    Nanoseconds timePassed = endTime - nStartTime;
    Nanoseconds remainingTime = nTargetFrameTimeNano - timePassed;

    // The highest precision sleep_for operates on are millisecond intervals, so
    // we convert our remaining time from nanoseconds to milliseconds.
    const int nanoInMilli = 1000000;
    Milliseconds remainingTimeMilli(remainingTime.count() / nanoInMilli);
    if (remainingTimeMilli.count() > 0)
    {
      std::this_thread::sleep_for(remainingTimeMilli);
    }
    endTime = Clock::now();
  }
  Nanoseconds fullTime = endTime - nStartTime;
  nDeltaTime = (float)fullTime.count() / (float)Nanoseconds::period::den;
}

float DeltaTime()
{
  return nDeltaTime;
}

} // namespace Framer