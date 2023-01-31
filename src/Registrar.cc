// Any component type must be registered within the RegisterTypes function. The
// order in which components are registered maps directly to their TypeIds. If a
// type is not registered in the RegisterTypes function, that type will not be
// usable as a component. Expect a crash if it's added to a Space Member.

#include "Registrar.h"

// clang-format off
#include "comp/Transform.h"
#include "comp/Mesh.h"
#include "comp/Sprite.h"
#include "comp/Text.h"
#include "comp/AlphaColor.h"
#include "comp/Camera.h"
#include "comp/DirectionalLight.h"
#include "comp/PointLight.h"
#include "comp/SpotLight.h"
#include "comp/Skybox.h"
#include "comp/ShadowMap.h"
#include "comp/Model.h"
// clang-format on

namespace Registrar {

void RegisterTypes()
{
  using namespace Comp;
  RegisterComponent(Transform);
  RegisterComponent(Mesh);
  RegisterDependencies(Mesh, Transform);
  RegisterComponent(Sprite);
  RegisterDependencies(Sprite, Transform);
  RegisterComponent(Text);
  RegisterDependencies(Text, Transform);
  RegisterComponent(AlphaColor);
  RegisterComponent(Camera);
  RegisterDependencies(Camera, Transform);
  RegisterComponent(DirectionalLight);
  RegisterComponent(PointLight);
  RegisterComponent(SpotLight);
  RegisterComponent(Skybox);
  RegisterComponent(ShadowMap);
  RegisterDependencies(ShadowMap, Camera);
  RegisterComponent(Model);
  RegisterDependencies(Model, Transform);
}

void (*nRegisterCustomTypes)() = nullptr;
void Init()
{
  RegisterTypes();
  if (nRegisterCustomTypes != nullptr) {
    nRegisterCustomTypes();
  }
  Comp::AssessComponentsFile();
}

} // namespace Registrar
