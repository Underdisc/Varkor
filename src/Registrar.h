#ifndef Registrar_h
#define Registrar_h

#include "editor/HookInterface.h"

namespace Registrar {

void Init();

template<typename T>
void Register()
{
  Comp::Type<T>::Register();
  Editor::RegisterHook<T>();
}

} // namespace Registrar

#endif
