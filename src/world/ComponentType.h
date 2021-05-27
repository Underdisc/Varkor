#ifndef world_ComponentData_h
#define world_ComponentData_h

#include "ds/Map.h"
#include "world/Types.h"

namespace World {

template<typename T>
struct ComponentType
{
  static void Register();
  static ComponentId smId;
};

struct ComponentData
{
  int mSize;
};

extern Ds::Map<ComponentId, ComponentData> nComponentTypeData;

} // namespace World

#include "ComponentType.hh"

#endif
