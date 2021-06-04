#include "comp/Model.h"
#include "comp/Transform.h"
#include "comp/Type.h"

namespace Comp {

void RegisterTypes()
{
  Type<Model>::Register();
  Type<Model>::RegisterEditHook();
  Type<Transform>::Register();
  Type<Transform>::RegisterEditHook();
  Type<Transform>::RegisterGizmo();
}

} // namespace Comp
