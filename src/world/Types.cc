#include "Types.h"

namespace World {

int CreateId()
{
  static int id = 0;
  return id++;
}

} // namespace World
