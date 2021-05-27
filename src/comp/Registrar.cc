#include "comp/Model.h"
#include "comp/Transform.h"
#include "world/ComponentType.h"
#include "world/Types.h"

namespace Comp {

void RegisterTypes()
{
  World::ComponentType<Model>::Register();
  World::ComponentType<Transform>::Register();
}

} // namespace Comp
