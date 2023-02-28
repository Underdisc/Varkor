#include "gfx/Renderable.h"
#include "Error.h"
#include "comp/Camera.h"
#include "ext/Tracy.h"
#include "gfx/Material.h"
#include "gfx/Mesh.h"
#include "math/Geometry.h"
#include "rsl/Library.h"
#include "world/Object.h"

namespace Gfx {

Collection* Collection::smActiveCollection = nullptr;

Collection::Collection()
{
  mSkybox.mOwner = World::nInvalidMemberId;
}

void Collection::Collect(const World::Space& space)
{
  ZoneScoped;

  // Collect all renderables from every component in the given space.
  smActiveCollection = this;
  for (Comp::TypeId typeId = 0; typeId < Comp::TypeDataCount(); ++typeId) {
    const Comp::TypeData& typeData = Comp::GetTypeData(typeId);
    if (!typeData.mVRenderable.Open()) {
      continue;
    }
    // todo: Add CObjects (const Object).
    Ds::Vector<World::MemberId> slice = space.Slice(typeId);
    for (World::MemberId memberId : slice) {
      void* component = space.GetComponent(typeId, memberId);
      World::Object owner(const_cast<World::Space*>(&space), memberId);
      typeData.mVRenderable.Invoke(component, owner);
    }
  }
  smActiveCollection = nullptr;
}

void Collection::Collect(const World::Object& object)
{
  // Collect all of the renderables on a specific object.
  smActiveCollection = this;
  for (Comp::TypeId typeId = 0; typeId < Comp::TypeDataCount(); ++typeId) {
    const Comp::TypeData& typeData = Comp::GetTypeData(typeId);
    if (!typeData.mVRenderable.Open()) {
      continue;
    }
    void* component = object.TryGetComponent(typeId);
    if (component != nullptr) {
      typeData.mVRenderable.Invoke(component, object);
    }
  }
  smActiveCollection = nullptr;
}

void Collection::Add(Renderable::Floater&& floater)
{
  VerifyActiveCollection();
  smActiveCollection->mFloaters.Push(std::move(floater));
}

void Collection::Use(Renderable::Skybox&& skybox)
{
  VerifyActiveCollection();
  LogErrorIf(
    smActiveCollection->HasSkybox(), "Previous Skybox renderable overwritten.");
  smActiveCollection->mSkybox = std::move(skybox);
}

void Collection::Add(Renderable::Icon&& icon)
{
  VerifyActiveCollection();
  smActiveCollection->mIcons.Push(std::move(icon));
}

void Collection::RenderFloaters()
{
  for (const Renderable::Floater& floater : mFloaters) {
    // Get all of the resources needed for rendering.
    const auto* material = Rsl::TryGetRes<Gfx::Material>(floater.mMaterialId);
    const auto* mesh = Rsl::TryGetRes<Gfx::Mesh>(floater.mMeshId);
    if (material == nullptr || mesh == nullptr) {
      return;
    }
    Gfx::Shader* shader = Rsl::TryGetRes<Gfx::Shader>(material->mShaderId);
    if (shader == nullptr) {
      return;
    }
    shader->Use();

    // Perform the render.
    int textureIndex = 0;
    mUniforms.Bind(*shader, &textureIndex);
    material->mUniforms.Bind(*shader, &textureIndex);
    floater.mUniforms.Bind(*shader, &textureIndex);
    shader->SetUniform("uModel", floater.mTransform);
    mesh->Render();
  }
}

bool Collection::HasSkybox() const
{
  return mSkybox.mOwner != World::nInvalidMemberId;
}

void Collection::RenderSkybox()
{
  LogAbortIf(!HasSkybox(), "Collection does not have a Skybox renderable.");
  const auto* material = Rsl::TryGetRes<Gfx::Material>(mSkybox.mMaterialId);
  if (material == nullptr) {
    return;
  }
  Gfx::Shader* shader = Rsl::TryGetRes<Gfx::Shader>(material->mShaderId);
  if (shader == nullptr) {
    return;
  }
  shader->Use();

  int textureIndex = 0;
  mUniforms.Bind(*shader, &textureIndex);
  material->mUniforms.Bind(*shader, &textureIndex);
  mSkybox.mUniforms.Bind(*shader, &textureIndex);

  const auto& skyboxMesh = Rsl::GetRes<Mesh>("vres/renderer:Skybox");
  skyboxMesh.Render();
}

void Collection::RenderIcons(
  bool memberIds, const World::Object& cameraObject) const
{
  Shader* shader;
  if (memberIds) {
    shader = &Rsl::GetRes<Shader>("vres/renderer:MemberId");
  }
  else {
    shader = &Rsl::GetRes<Shader>("vres/renderer:Color");
  }
  shader->Use();

  const auto& cameraComp = cameraObject.Get<Comp::Camera>();
  Quat iconRotation = cameraComp.WorldRotation(cameraObject);
  Mat4 iconRotate;
  Math::Rotate(&iconRotate, iconRotation);
  for (const Renderable::Icon& icon : mIcons) {
    Mesh* mesh = Rsl::TryGetRes<Mesh>(icon.mMeshId);
    if (mesh == nullptr) {
      return;
    }

    // Set the model transformation.
    float uniformScale =
      cameraComp.ProjectedDistance(cameraObject, icon.mTranslation);
    if (uniformScale == 0.0f) {
      continue;
    }
    uniformScale *= 0.02f;
    Mat4 scale, translate;
    Math::Scale(&scale, uniformScale);
    Math::Translate(&translate, icon.mTranslation);
    Mat4 transformation = translate * iconRotate * scale;
    shader->SetUniform("uModel", transformation);

    if (memberIds) {
      shader->SetUniform("uMemberId", icon.mOwner);
    }
    else {
      shader->SetUniform("uColor", icon.mColor);
    }

    mesh->Render();
  }
}

void Collection::VerifyActiveCollection()
{
  LogAbortIf(smActiveCollection == nullptr, "Active collection not set.");
}

} // namespace Gfx