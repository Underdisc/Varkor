// Any component type must be registered within the RegisterTypes function. The
// order in which components are registered maps directly to their TypeIds. If
// that component happens to have an editor hook, that hook's header must be
// included here.

// This is what happens if these bureaucratic rules are not followed.
// - If a type is not registered in the RegisterTypes function, that type will
// not be usable as a component. Expect a crash if it's added to a Space Member.
// - If a registered component has an editor hook, but the hook's header is not
// included here, there will be a linker error because there will be multiple
// definitions for the editor hook. Component's are given default editor hooks
// that do nothing if the hook is not declared.

#include "Registrar.h"
#include "comp/Type.h"
#include "editor/Interface.h"

// clang-format off
#include "comp/Model.h"
#include "comp/Transform.h"
#include "comp/Sprite.h"

#include "editor/hook/Model.h"
#include "editor/hook/Transform.h"
#include "editor/hook/Sprite.h"
// clang-format on

namespace Registrar {

template<typename T>
void Register()
{
  Comp::Type<T>::Register();
  Editor::RegisterHook<T>();
}

void RegisterTypes()
{
  Register<Comp::Model>();
  Register<Comp::Transform>();
  Register<Comp::Sprite>();
}

void Init()
{
  RegisterTypes();
  Comp::ReadComponentsFile();
}

} // namespace Registrar
