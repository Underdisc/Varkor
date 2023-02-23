#include "gfx/Renderable.h"
#include "gfx/Material.h"
#include "gfx/Mesh.h"
#include "math/Geometry.h"
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

void Renderable::Collection::AddIcon(IconRenderable&& renderable)
{
  LogAbortIf(smActiveCollection == nullptr, "Active collection not set.");
  smActiveCollection->mIcons.Push(std::move(renderable));
}

void Renderable::Collection::Render(Type renderableType) const
{
  for (const Renderable& renderable : mRenderables[(size_t)renderableType]) {
    renderable.Render(mUniforms);
  }
}

void Renderable::Collection::RenderIcons(
  bool memberIds, const Mat4& view, const Mat4& proj, const Vec3& viewPos) const
{
  // Calculate values needed for computing model transformations.
  Mat4 cameraRotation = Math::HomogeneousOrthogonalInverse(view);
  for (int i = 0; i < 3; ++i) {
    cameraRotation[i][3] = 0.0f;
  }
  Vec3 viewForward = {0.0f, 0.0f, -1.0f};
  Mat3 orientation;
  Math::Resize(&orientation, cameraRotation);
  viewForward = orientation * viewForward;

  Shader* shader;
  if (memberIds) {
    shader = &Rsl::GetRes<Shader>("vres/renderer:MemberId");
  }
  else {
    shader = &Rsl::GetRes<Shader>("vres/renderer:Color");
  }
  shader->Use();

  for (const IconRenderable& icon : mIcons) {
    Mesh* mesh = Rsl::TryGetRes<Mesh>(icon.mMeshId);
    if (mesh == nullptr) {
      return;
    }

    // todo: Getting a scaling factor depending on the view ray like this is
    // also necessary for the gizmos. They should use the same function.
    Math::Ray viewRay;
    viewRay.StartDirection(viewPos, viewForward);
    Vec3 projection = viewRay.ClosestPointTo(icon.mTranslation);
    Vec3 distance = projection - viewPos;
    if (Math::Near(distance, {0.0f, 0.0f, 0.0f})) {
      continue;
    }
    float uniformScale = Math::Magnitude(distance) * 0.05f;

    // Set the model transformation.
    Mat4 scale, translate;
    Math::Scale(&scale, uniformScale);
    Math::Translate(&translate, icon.mTranslation);
    Mat4 transformation = translate * cameraRotation * scale;
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

const Ds::Vector<Renderable>& Renderable::Collection::Get(Type type) const
{
  return mRenderables[(size_t)type];
}

} // namespace Gfx