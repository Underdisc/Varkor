#ifndef Registrar_h
#define Registrar_h

#include "editor/HookInterface.h"

namespace Registrar {

extern void (*nRegisterCustomTypes)();
void Init();

template<typename T, typename... Dependencies>
void Register()
{
  Comp::Type<T>::template Register<Dependencies...>();
  const Comp::TypeData& typeData = Comp::GetTypeData(Comp::Type<T>::smId);
  if (typeData.mVStaticInit.Open()) {
    typeData.mVStaticInit.Invoke(nullptr);
  }
  Editor::RegisterHook<T>();
}

} // namespace Registrar

#endif
