#include <glad/glad.h>

#include "comp/Mesh.h"
#include "comp/Transform.h"
#include "editor/AssetInterfaces.h"
#include "gfx/Material.h"
#include "gfx/Mesh.h"
#include "gfx/Renderable.h"

namespace Comp {

void Mesh::VInit(const World::Object& owner)
{
  mMeshId = Rsl::GetDefaultResId<Gfx::Mesh>();
  mMaterialId = Rsl::GetDefaultResId<Gfx::Material>();
  mVisible = true;
}

void Mesh::VSerialize(Vlk::Value& val)
{
  val("MeshId") = mMeshId;
  val("MaterialId") = mMaterialId;
  val("Visible") = mVisible;
}

void Mesh::VDeserialize(const Vlk::Explorer& modelEx)
{
  mMeshId = modelEx("MeshId").As<ResId>(Rsl::GetDefaultResId<Gfx::Mesh>());
  mMaterialId =
    modelEx("MaterialId").As<ResId>(Rsl::GetDefaultResId<Gfx::Material>());
  mVisible = modelEx("Visible").As<bool>(true);
}

void Mesh::VRenderable(const World::Object& owner)
{
  if (!mVisible) {
    return;
  }
  Gfx::Renderable renderable;
  renderable.mOwner = owner.mMemberId;
  renderable.mTransform = owner.Get<Transform>().GetWorldMatrix(owner);
  renderable.mMaterialId = mMaterialId;
  renderable.mMeshId = mMeshId;
  Gfx::Renderable::Collection::Add(
    Gfx::Renderable::Type::Floater, std::move(renderable));
}

void Mesh::VEdit(const World::Object& owner)
{
  Editor::DropResourceWidget<Gfx::Mesh>(&mMeshId);
  Editor::DropResourceWidget<Gfx::Material>(&mMaterialId);
  ImGui::Checkbox("Visible", &mVisible);
}

} // namespace Comp
