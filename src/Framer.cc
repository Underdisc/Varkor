#if defined WIN32
  #include <windows.h>
  #pragma comment(lib, "Winmm.lib")
#endif

#include <GLFW/glfw3.h>
#include <chrono>
#include <thread>

#include "ds/Vector.h"

#include "Framer.h"

namespace Framer {

void CalculateAverages();

// todo: When working on creating linux builds, you will need to check the
// precision of the clock. The precision of steady_clock under msvc 1927 is
// 10^-7. Do not use std::chrono::high_resolution_clock. It is usually just an
// alias for steady_clock or system_clock.
typedef std::chrono::steady_clock Clock;
typedef std::chrono::nanoseconds Nanoseconds;
typedef std::chrono::milliseconds Milliseconds;

// The target framerate when software based frame limiting is active. When
// software frame limiting is not active, this will have a unique value to
// represent the current state of the framer.
int nFramerate;
constexpr int nVSyncValue = -1;
constexpr int nUncappedValue = 0;
constexpr int nDefaultFramerate = 60;
Nanoseconds nTargetFrameTimeNano;

float nDeltaTime = 0.0f;
float nTotalTime = 0.0f;
Clock::time_point nStartTime;

// These are used for tracking and updating average fps and frame usage values.
Ds::Vector<float> nPreviousFrameUsages;
Ds::Vector<float> nPreviousFpsValues;
float nAverageFrameUsage;
float nAverageFps;
float nAverageUpdatePeriod = 1.0f;
Clock::time_point nLastAverageUpdateTime;

void Init()
{
#if defined WIN32
  // This gives the Window's sleep function millisecond accuracy.
  timeBeginPeriod(1);
#endif

  SetVSync(true);
  SetFrameAverageCount(10);
  nLastAverageUpdateTime = Clock::now();
}

void Purge()
{
#if defined WIN32
  timeEndPeriod(1);
#endif
}

void Start()
{
  nStartTime = Clock::now();
}

// sleep_for has millisecond precision. This lack of precision will result in
// framerates that do not stick to the desired framerate. For example, setting
// 120 as the framerate could result in 125 fps or 111 fps. 120 fps
// requires 8.333 ms frame times, but since sleep_for has millisecond precision
// this will result in either 8 ms (125 fps) or 9 ms (111 fps) frames. This is
// the behaviour under msvc 1927.
void End()
{
  Clock::time_point endTime = Clock::now();
  if (!VSyncEnabled() && !FramerateUncapped())
  {
    Nanoseconds timePassed = endTime - nStartTime;
    Nanoseconds remainingTime = nTargetFrameTimeNano - timePassed;

    // The highest precision sleep_for operates on are millisecond intervals, so
    // we convert our remaining time from nanoseconds to milliseconds.
    constexpr int nanoInMilli = 1000000;
    Milliseconds remainingTimeMilli(remainingTime.count() / nanoInMilli);
    if (remainingTimeMilli.count() > 0)
    {
      std::this_thread::sleep_for(remainingTimeMilli);
    }
    endTime = Clock::now();

    float frameUsage =
      (float)timePassed.count() / (float)nTargetFrameTimeNano.count();
    nPreviousFrameUsages.Pop();
    nPreviousFrameUsages.Insert(0, frameUsage);
  } else
  {
    nPreviousFrameUsages.Pop();
    nPreviousFrameUsages.Insert(0, 1.0f);
  }

  Nanoseconds fullTime = endTime - nStartTime;
  nDeltaTime = (float)fullTime.count() / (float)Nanoseconds::period::den;
  float fps = 1.0f / nDeltaTime;
  nPreviousFpsValues.Pop();
  nPreviousFpsValues.Insert(0, fps);
  nTotalTime += nDeltaTime;

  Nanoseconds sinceAverageUpdate = endTime - nLastAverageUpdateTime;
  float AverageUpdateDelta =
    (float)sinceAverageUpdate.count() / (float)Nanoseconds::period::den;
  if (AverageUpdateDelta >= nAverageUpdatePeriod)
  {
    CalculateAverages();
    nLastAverageUpdateTime = Clock::now();
  }
}

void SetVSync(bool active)
{
  if (active)
  {
    glfwSwapInterval(1);
    nFramerate = nVSyncValue;
    nTargetFrameTimeNano = Nanoseconds(0);
    return;
  }

  glfwSwapInterval(0);
  nFramerate = nDefaultFramerate;
  nTargetFrameTimeNano =
    Nanoseconds(Nanoseconds::period::den / nDefaultFramerate);
}

void SetFramerate(int framerate)
{
  LogAbortIf(VSyncEnabled(), "VSync must be disabled to set a framerate.");
  if (framerate == nUncappedValue)
  {
    nFramerate = nUncappedValue;
    nTargetFrameTimeNano = Nanoseconds(0);
    return;
  }
  nFramerate = framerate;
  nTargetFrameTimeNano = Nanoseconds(Nanoseconds::period::den / framerate);
}

void SetDefaultFramerate()
{
  SetFramerate(nDefaultFramerate);
}

void SetFrameAverageCount(int count)
{
  nPreviousFrameUsages.Resize(count, 0.0f);
  nPreviousFrameUsages.Shrink();
  nPreviousFpsValues.Resize(count, 0.0f);
  nPreviousFpsValues.Shrink();
}

bool VSyncEnabled()
{
  return nFramerate == nVSyncValue;
}

bool FramerateUncapped()
{
  return nFramerate == nUncappedValue;
}

float DeltaTime()
{
  return nDeltaTime;
}

float TotalTime()
{
  return nTotalTime;
}

float GetAverageFrameUsage()
{
  return nAverageFrameUsage;
}

float GetAverageFps()
{
  return nAverageFps;
}

int GetFramerate()
{
  return nFramerate;
}

void CalculateAverages()
{
  nAverageFrameUsage = 0.0f;
  for (float prevFrameUsage : nPreviousFrameUsages)
  {
    nAverageFrameUsage += prevFrameUsage;
  }
  nAverageFrameUsage /= nPreviousFrameUsages.Size();

  nAverageFps = 0.0f;
  for (float prevFpsValue : nPreviousFpsValues)
  {
    nAverageFps += prevFpsValue;
  }
  nAverageFps /= nPreviousFpsValues.Size();
}

} // namespace Framer