#ifndef framer_h
#define framer_h

#include <chrono>

class Framer
{
public:
  static void Init(int framerate);
  static void SetFramerate(int framerate);
  static void Start();
  static void End();

  typedef std::chrono::system_clock system_clock;
  typedef std::chrono::time_point<system_clock> time_point;
  // float
  time_point _frame_start;
  time_point _frame_end;
};

#endif