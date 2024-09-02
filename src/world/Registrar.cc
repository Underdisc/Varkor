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
void (*nComponentProgressions[nCurrentComponentProgression + 1])(
  Vlk::Value& componentsVal);
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
void ComponentProgression(Vlk::Value& componentsVal);

template<int N>
void AssignComponentProgressions()
{
  nComponentProgressions[N] = ComponentProgression<N>;
  AssignComponentProgressions<N - 1>();
}

template<>
void AssignComponentProgressions<nInvalidProgression>()
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
  Vlk::Value& spaceVal = layerVal.GetPair("Space");
  for (int i = 0; i < spaceVal.Size(); ++i) {
    Vlk::Value& entityVal = spaceVal[i];
    Vlk::Value& nameVal = entityVal.GetPair("Name");
    Vlk::Value& componentsVal = entityVal.GetPair("Components");
    componentsVal("Name") = nameVal.As<std::string>();
    entityVal.RemovePair("Name");
  }
}

template<>
void LayerProgression<1>(Vlk::Value& layerVal)
{
  Vlk::Value& metadataVal = layerVal.GetPair("Metadata");
  Vlk::Value& progressionVal = metadataVal.GetPair("Progression");
  metadataVal("ComponentProgression") = progressionVal.As<int>();
  metadataVal.RemovePair("Progression");
}

template<>
void ComponentProgression<0>(Vlk::Value& componentsVal)
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
void ComponentProgression<1>(Vlk::Value& componentsVal)
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
void ComponentProgression<2>(Vlk::Value& componentsVal)
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
void ComponentProgression<3>(Vlk::Value& componentsVal)
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
  AssignComponentProgressions<nCurrentComponentProgression>();
  AssignLayerProgressions<nCurrentLayerProgression>();
}

void ProgressLayer(Vlk::Value& layerValue, int startProgression)
{
  for (int i = startProgression + 1; i <= nCurrentLayerProgression; ++i) {
    nLayerProgressions[i](layerValue);
  }
}

void ProgressComponents(Vlk::Value& spaceVal, int startComponentProgression)
{
  int i = startComponentProgression + 1;
  for (; i <= nCurrentComponentProgression; ++i) {
    for (int j = 0; j < spaceVal.Size(); ++j) {
      Vlk::Value& componentsVal = spaceVal[j]("Components");
      nComponentProgressions[i](componentsVal);
    }
  }
}

} // namespace Registrar
