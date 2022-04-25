#ifndef editor_hook_Sprite_h
#define editor_hook_Sprite_h

#include "comp/Sprite.h"
#include "editor/HookInterface.h"

namespace Editor {

template<>
struct Hook<Comp::Sprite>: public HookInterface
{
  void Edit(const World::Object& object);
};

} // namespace Editor

#endif
