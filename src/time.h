#ifndef Time_h
#define Time_h

#include <chrono>
#include <ctime>

#include <unistd>

class Time
{
public:
    static void Init();
    static void Update();
    static float DeltaTime();

    void temp()
    {
        sleep
        time_t time;
    }


private:
    // We need a start time.
    // We need dt.
    // So we need a time since start.

    typedef std::chrono::system_clock system_clock;
    typedef std::chrono::time_point<system_clock> time_point;

};

#endif