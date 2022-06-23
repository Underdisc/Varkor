// Any component type must be registered within the RegisterTypes function. The
// order in which components are registered maps directly to their TypeIds. If a
// type is not registered in the RegisterTypes function, that type will not be
// usable as a component. Expect a crash if it's added to a Space Member.

#include "Registrar.h"
#include "comp/Type.h"

// clang-format off
#include "comp/Transform.h"
#include "comp/Model.h"
#include "comp/Sprite.h"
#include "comp/Text.h"
#include "comp/AlphaColor.h"
#include "comp/Camera.h"
#include "comp/DirectionalLight.h"
#include "comp/PointLight.h"
#include "comp/SpotLight.h"
#include "comp/Skybox.h"
#include "comp/ShadowMap.h"
#include "comp/DefaultPostProcess.h"
#include "comp/ExposurePostProcess.h"
// clang-format on

namespace Registrar {

void RegisterTypes()
{
  using namespace Comp;
  Type<Transform>::Register();
  Type<Model>::Register();
  Type<Model>::AddDependencies<Transform>();
  Type<Sprite>::Register();
  Type<Sprite>::AddDependencies<Transform>();
  Type<Text>::Register();
  Type<Text>::AddDependencies<Transform>();
  Type<AlphaColor>::Register();
  Type<Camera>::Register();
  Type<Camera>::AddDependencies<Transform>();
  Type<DirectionalLight>::Register();
  Type<PointLight>::Register();
  Type<SpotLight>::Register();
  Type<Skybox>::Register();
  Type<ShadowMap>::Register();
  Type<ShadowMap>::AddDependencies<Camera>();
  Type<DefaultPostProcess>::Register();
  Type<ExposurePostProcess>::Register();
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
