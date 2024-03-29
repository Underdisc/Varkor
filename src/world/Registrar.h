#ifndef Registrar_h
#define Registrar_h

#include "comp/Type.h"

#define RegisterComponent(name) Comp::Type<name>::Register(#name)
#define RegisterDependencies(name, ...) \
  Comp::Type<name>::AddDependencies<__VA_ARGS__>()

namespace Registrar {

constexpr int nInvalidProgression = -1;
constexpr int nCurrentProgression = 2;

extern void (*nRegisterCustomTypes)();
void Init();
void ProgressComponents(Vlk::Value& spaceVal, int startProgression);

} // namespace Registrar

#endif
