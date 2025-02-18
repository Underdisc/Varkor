#include "gfx/Renderable.h"
#include "Error.h"
#include "comp/Camera.h"
#include "ext/Tracy.h"
#include "gfx/Material.h"
#include "gfx/Mesh.h"
#include "rsl/Library.h"
#include "world/Object.h"

namespace Gfx {

Collection* Collection::smActiveCollection = nullptr;

Collection::Collection() {
  mSkybox.mOwner = World::nInvalidMemberId;
}

void Collection::Collect(const World::Space& space) {
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
    for (World::MemberId memberId: slice) {
      void* component = space.GetComponent(typeId, memberId);
      World::Object owner(const_cast<World::Space*>(&space), memberId);
      typeData.mVRenderable.Invoke(component, owner);
    }
  }
  smActiveCollection = nullptr;
}

void Collection::Collect(const World::Object& object) {
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

void Collection::Add(Renderable::Floater&& floater) {
  VerifyActiveCollection();
  smActiveCollection->mFloaters.Push(std::move(floater));
}

void Collection::Use(Renderable::Skybox&& skybox) {
  VerifyActiveCollection();
  LogErrorIf(
    smActiveCollection->HasSkybox(), "Previous Skybox renderable overwritten.");
  smActiveCollection->mSkybox = std::move(skybox);
}

void Collection::Add(Renderable::Icon&& icon) {
  VerifyActiveCollection();
  smActiveCollection->mIcons.Push(std::move(icon));
}

void Collection::RenderFloaters() {
  ResId currentMaterialId;
  int currentTextureIndex;
  Shader* currentShader = nullptr;
  for (const Renderable::Floater& floater: mFloaters) {
    // Bind a new material if the material changes.
    if (floater.mMaterialId != currentMaterialId) {
      const auto* nextMaterial =
        Rsl::TryGetRes<Gfx::Material>(floater.mMaterialId);
      if (nextMaterial == nullptr) {
        continue;
      }
      Gfx::Shader* nextShader =
        Rsl::TryGetRes<Gfx::Shader>(nextMaterial->mShaderId);
      if (nextShader == nullptr) {
        continue;
      }
      currentMaterialId = floater.mMaterialId;
      currentTextureIndex = 0;
      currentShader = nextShader;
      currentShader->Use();
      mUniforms.Bind(*currentShader, &currentTextureIndex);
      nextMaterial->mUniforms.Bind(*currentShader, &currentTextureIndex);
    }

    // Ensure that rendering is possible.
    if (currentShader == nullptr) {
      continue;
    }
    const auto* mesh = Rsl::TryGetRes<Gfx::Mesh>(floater.mMeshId);
    if (mesh == nullptr) {
      continue;
    }

    // Perform the render.
    int textureIndex = currentTextureIndex;
    floater.mUniforms.Bind(*currentShader, &textureIndex);
    currentShader->SetUniform("uModel", floater.mTransform);
    mesh->Render();
  }
}

bool Collection::HasSkybox() const {
  return mSkybox.mOwner != World::nInvalidMemberId;
}

void Collection::RenderSkybox() {
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
  bool memberIds, const World::Object& cameraObject) const {
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
  for (const Renderable::Icon& icon: mIcons) {
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

void Collection::VerifyActiveCollection() {
  LogAbortIf(smActiveCollection == nullptr, "Active collection not set.");
}

} // namespace Gfx
