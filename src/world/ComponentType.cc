#include "ComponentType.h"

namespace World {

Ds::Map<ComponentId, ComponentData> nComponentTypeData;

int CreateId()
{
  static int id = 0;
  return id++;
}

} // namespace World
