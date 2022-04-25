#ifndef editor_hook_Text_h
#define editor_hook_Text_h

#include "comp/Text.h"
#include "editor/HookInterface.h"

namespace Editor {

template<>
struct Hook<Comp::Text>: public HookInterface
{
  void Edit(const World::Object& object);
};

} // namespace Editor

#endif
