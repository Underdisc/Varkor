#ifndef editor_hook_Model_h
#define editor_hook_Model_h

#include "Hook.h"
#include "comp/Model.h"

namespace Editor {
namespace Hook {

template<>
void Edit(Comp::Model* model);

} // namespace Hook
} // namespace Editor

#endif
