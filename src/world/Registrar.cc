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
#include "comp/CameraOrbiter.h"
#include "comp/BoxCollider.h"
#include "comp/Name.h"
// clang-format on

namespace Registrar {

void (*nRegisterCustomTypes)() = nullptr;
void (*nProgressions[nCurrentProgression + 1])(Vlk::Value& componentsVal);
void (*nLayerProgressions[nCurrentLayerProgression + 1])(Vlk::Value& layerVal);

template<int N>
void LayerProgression(Vlk::Value& layerVal);

template<int N>
void AssignLayerProgressions()
{
  nLayerProgressions[N] = LayerProgression<N>;
  AssignLayerProgressions<N - 1>();
}

template<>
void AssignLayerProgressions<nInvalidProgression>()
{}

template<int N>
void Progression(Vlk::Value& componentsVal);

template<int N>
void AssignProgressions()
{
  nProgressions[N] = Progression<N>;
  AssignProgressions<N - 1>();
}

template<>
void AssignProgressions<nInvalidProgression>()
{}

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
  RegisterDependencies(SpotLight, Transform);
  RegisterComponent(Skybox);
  RegisterDependencies(Skybox, Transform);
  RegisterComponent(ShadowMap);
  RegisterDependencies(ShadowMap, Camera);
  RegisterComponent(Model);
  RegisterDependencies(Model, Transform);
  RegisterComponent(CameraOrbiter);
  RegisterDependencies(CameraOrbiter, Camera);
  RegisterComponent(BoxCollider);
  RegisterComponent(Name);
}

template<>
void LayerProgression<0>(Vlk::Value& layerVal)
{
  Vlk::Value* spaceVal = layerVal.TryGetPair("Space");
  for (int i = 0; i < spaceVal->Size(); ++i) {
    Vlk::Value& entityVal = (*spaceVal)[i];
    Vlk::Value& nameVal = entityVal("Name");
    Vlk::Value& componentsVal = entityVal("Components");
    componentsVal("Name") = nameVal.As<std::string>();
    entityVal.TryRemovePair("Name");
  }
}

template<>
void Progression<0>(Vlk::Value& componentsVal)
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

template<>
void Progression<1>(Vlk::Value& componentsVal)
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

template<>
void Progression<2>(Vlk::Value& componentsVal)
{
  Vlk::Value* spotLightVal = componentsVal.TryGetPair("SpotLight");
  if (spotLightVal == nullptr) {
    return;
  }

  Vec3 position = (*spotLightVal)("Position").As<Vec3>({0.0f, 0.0f, 0.0f});
  spotLightVal->TryRemovePair("Position");
  Vec3 direction = (*spotLightVal)("Direction").As<Vec3>({0.0f, -1.0f, 0.0f});
  spotLightVal->TryRemovePair("Direction");

  Vlk::Value& transformVal = componentsVal("Transform");
  transformVal("Translation") = position;
  Quat rotation;
  rotation.FromTo({1.0f, 0.0f, 0.0f}, direction);
  transformVal("Rotation") = rotation;
}

template<>
void Progression<3>(Vlk::Value& componentsVal)
{
  Vlk::Value* cameraOrbiterVal = componentsVal.TryGetPair("CameraOrbiter");
  if (cameraOrbiterVal == nullptr) {
    return;
  }

  float rate = (*cameraOrbiterVal)("Rate").As<float>(0.2f);
  (*cameraOrbiterVal)("Period") = 1 / rate;
}

void Init()
{
  RegisterTypes();
  if (nRegisterCustomTypes != nullptr) {
    nRegisterCustomTypes();
  }
  Comp::AssessComponentsFile();
  AssignProgressions<nCurrentProgression>();
  AssignLayerProgressions<nCurrentLayerProgression>();
}

void ProgressLayer(Vlk::Value& layerValue, int startProgression)
{
  for (int i = startProgression + 1; i <= nCurrentLayerProgression; ++i) {
    nLayerProgressions[i](layerValue);
  }
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
