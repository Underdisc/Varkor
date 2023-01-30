#ifndef Registrar_h
#define Registrar_h

#include "comp/Type.h"

#define RegisterComponent(name) Comp::Type<name>::Register(#name)
#define RegisterDependencies(name, ...) \
  Comp::Type<name>::AddDependencies<__VA_ARGS__>()

namespace Registrar {

extern void (*nRegisterCustomTypes)();
void Init();

} // namespace Registrar

#endif
