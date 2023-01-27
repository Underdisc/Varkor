#ifndef Registrar_h
#define Registrar_h

#include "comp/Type.h"

#define RegisterComponent(name) Type<name>::Register(#name)
#define RegisterDependencies(name, ...) \
  Type<name>::AddDependencies<__VA_ARGS__>()

namespace Registrar {

extern void (*nRegisterCustomTypes)();
void Init();

} // namespace Registrar

#endif
