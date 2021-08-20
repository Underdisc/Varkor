#ifndef editor_hook_Model_h
#define editor_hook_Model_h

#include "comp/Model.h"
#include "editor/HookInterface.h"

namespace Editor {

template<>
struct Hook<Comp::Model>: public HookInterface
{
  bool Edit(const World::Object& object);
};

} // namespace Editor

#endif
