#ifndef Registrar_h
#define Registrar_h

#include "comp/Type.h"

#define RegisterComponent(name) Comp::Type<name>::Register(#name)
#define RegisterDependencies(name, ...) \
  Comp::Type<name>::AddDependencies<__VA_ARGS__>()

namespace Registrar {

constexpr int nInvalidProgression = -1;
constexpr int nCurrentComponentProgression = 3;
constexpr int nCurrentLayerProgression = 2;

extern void (*nRegisterCustomTypes)();
void Init();
void ProgressComponents(Vlk::Value& spaceVal, int startComponentProgression);
void ProgressLayer(Vlk::Value& layerVal, int startLayerProgression);

} // namespace Registrar

#endif
