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

// clang-format off
#include "comp/Transform.h"
#include "comp/Model.h"
#include "comp/Sprite.h"
#include "comp/Text.h"
#include "comp/AlphaColor.h"

#include "editor/hook/Transform.h"
#include "editor/hook/Model.h"
#include "editor/hook/Sprite.h"
#include "editor/hook/Text.h"
// clang-format on

namespace Registrar {

void RegisterTypes()
{
  using namespace Comp;
  Register<Transform>();
  Register<Model, Transform>();
  Register<Sprite, Transform>();
  Register<Text, Transform>();
  Register<AlphaColor>();
}

void (*nRegisterCustomTypes)() = nullptr;
void Init()
{
  RegisterTypes();
  if (nRegisterCustomTypes != nullptr)
  {
    nRegisterCustomTypes();
  }
  Comp::AssessComponentsFile();
}

} // namespace Registrar
