#include "Types.h"

namespace Core {

int CreateId()
{
  static int id = 0;
  return id++;
}

} // namespace Core
