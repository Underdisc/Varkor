// Any component type must be registered within the RegisterTypes function. The
// order in which components are registered maps directly to their TypeIds. If a
// type is not registered in the RegisterTypes function, that type will not be
// usable as a component. Expect a crash if it's added to a Space Member.

#include "Registrar.h"
#include "vlk/Valkor.h"

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

void (*nRegisterCustomTypes)() = nullptr;
void (*nProgressions[1])(Vlk::Value& componentsVal);

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
  RegisterDependencies(DirectionalLight, Transform);
  RegisterComponent(PointLight);
  RegisterDependencies(PointLight, Transform);
  RegisterComponent(SpotLight);
  RegisterComponent(Skybox);
  RegisterComponent(ShadowMap);
  RegisterDependencies(ShadowMap, Camera);
  RegisterComponent(Model);
  RegisterDependencies(Model, Transform);
}

void Progression0(Vlk::Value& componentsVal)
{
  Vlk::Value* directionLightVal = componentsVal.TryGetPair("DirectionalLight");
  if (directionLightVal == nullptr) {
    return;
  }

  Vec3 direction =
    (*directionLightVal)("Direction").As<Vec3>({0.0f, -1.0f, 0.0f});
  directionLightVal->TryRemovePair("Direction");

  Vlk::Value& transformVal = componentsVal("Transform");
  Quat rotation;
  rotation.FromTo({1.0f, 0.0f, 0.0f}, direction);
  transformVal("Rotation") = rotation;
}

void Progression1(Vlk::Value& componentsVal)
{
  Vlk::Value* pointLightVal = componentsVal.TryGetPair("PointLight");
  if (pointLightVal == nullptr) {
    return;
  }

  Vec3 position = (*pointLightVal)("Position").As<Vec3>({0.0f, 0.0f, 0.0f});
  pointLightVal->TryRemovePair("Position");

  Vlk::Value& transformVal = componentsVal("Transform");
  transformVal("Translation") = position;
}

void Init()
{
  RegisterTypes();
  if (nRegisterCustomTypes != nullptr) {
    nRegisterCustomTypes();
  }
  Comp::AssessComponentsFile();

  nProgressions[0] = Progression0;
  nProgressions[1] = Progression1;
}

void ProgressComponents(Vlk::Value& spaceVal, int startProgression)
{
  for (int i = startProgression + 1; i <= nCurrentProgression; ++i) {
    for (int j = 0; j < spaceVal.Size(); ++j) {
      Vlk::Value& componentsVal = spaceVal[j]("Components");
      nProgressions[i](componentsVal);
    }
  }
}

} // namespace Registrar
