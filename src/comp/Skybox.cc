#include "comp/Skybox.h"
#include "editor/Utility.h"
#include "gfx/Renderer.h"
#include "rsl/Library.h"

namespace Comp {

const ResId Skybox::smDefaultMaterialId(Skybox::smDefaultAssetName, "Default");

void Skybox::VStaticInit()
{
  Rsl::RequireAsset(smDefaultAssetName);
}

void Skybox::VInit(const World::Object& owner)
{
  mMaterialId = smDefaultMaterialId;
}

void Skybox::VSerialize(Vlk::Value& val)
{
  val("MaterialId") = mMaterialId;
}

void Skybox::VDeserialize(const Vlk::Explorer& ex)
{
  mMaterialId = ex("MaterialId").As<ResId>(smDefaultMaterialId);
}

void Skybox::VRenderable(const World::Object& owner)
{
  Gfx::Renderable renderable;
  renderable.mOwner = owner.mMemberId;
  Math::Identity(&renderable.mTransform);
  renderable.mMeshId = Gfx::Renderer::nSkyboxMeshId;
  renderable.mMaterialId = mMaterialId;
  Gfx::Renderable::Collection::Add(
    Gfx::Renderable::Type::Skybox, std::move(renderable));
}

void Skybox::VEdit(const World::Object& owner)
{
  Editor::DropResourceIdWidget(Rsl::ResTypeId::Material, &mMaterialId);
}

} // namespace Comp