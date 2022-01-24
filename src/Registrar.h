#ifndef Registrar_h
#define Registrar_h

#include "editor/HookInterface.h"

namespace Registrar {

extern void (*nRegisterCustomTypes)();
void Init();

template<typename T, typename... Dependencies>
void Register()
{
  Comp::Type<T>::Register<Dependencies...>();
  Editor::RegisterHook<T>();
}

} // namespace Registrar

#endif
