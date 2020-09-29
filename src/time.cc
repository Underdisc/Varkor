#include <chrono>

#include "framer.h"

#include "time.h"

namespace Time {

float DeltaTime()
{
  return Framer::DeltaTime();
}

} // namespace Time
