#ifndef editor_hook_Transform_h
#define editor_hook_Transform_h

#include "Hook.h"
#include "comp/Transform.h"

namespace Editor {
namespace Hook {

template<>
void Edit(Comp::Transform* transform);

} // namespace Hook
} // namespace Editor

#endif
