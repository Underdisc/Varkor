#include <Tracy.hpp>
#include <TracyC.h>

#ifndef TRACY_ENABLE
#define ProfileThread(name)
#else
#define ProfileThread(name) tracy::SetThreadName(name)
#endif
