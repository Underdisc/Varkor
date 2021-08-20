#include "editor/HookInterface.h"

namespace Editor {

HookInterface::~HookInterface() {}

Ds::Vector<HookFunctions> nHookFunctions;
const HookFunctions& GetHookFunctions(Comp::TypeId id)
{
  return nHookFunctions[id];
}

} // namespace Editor
