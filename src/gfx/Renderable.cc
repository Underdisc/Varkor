#include "gfx/Renderable.h"
#include "gfx/Material.h"
#include "gfx/Mesh.h"
#include "rsl/Library.h"
#include "world/Object.h"

namespace Gfx {

void Renderable::Render(const UniformVector& collectionUniforms) const
{
  // Get all of the resources needed for rendering.
  const auto* material = Rsl::TryGetRes<Gfx::Material>(mMaterialId);
  const auto* mesh = Rsl::TryGetRes<Gfx::Mesh>(mMeshId);
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
  collectionUniforms.Bind(*shader, &textureIndex);
  material->mUniforms.Bind(*shader, &textureIndex);
  mUniforms.Bind(*shader, &textureIndex);
  shader->SetUniform("uModel", mTransform);
  mesh->Render();
}

Renderable::Collection* Renderable::Collection::smActiveCollection = nullptr;

void Renderable::Collection::Collect(const World::Space& space)
{
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

void Renderable::Collection::Collect(const World::Object& object)
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

void Renderable::Collection::Add(Type renderableType, Renderable&& renderable)
{
  LogAbortIf(smActiveCollection == nullptr, "Active collection not set.");
  smActiveCollection->mRenderables[(size_t)renderableType].Push(
    std::move(renderable));
}

void Renderable::Collection::Render(Type renderableType) const
{
  for (const Renderable& renderable : mRenderables[(size_t)renderableType]) {
    renderable.Render(mUniforms);
  }
}

const Ds::Vector<Renderable>& Renderable::Collection::Get(Type type) const
{
  return mRenderables[(size_t)type];
}

} // namespace Gfx