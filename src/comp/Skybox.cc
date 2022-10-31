#include "comp/Skybox.h"
#include "editor/AssetInterfaces.h"
#include "gfx/Cubemap.h"
#include "gfx/Material.h"
#include "gfx/Renderer.h"
#include "gfx/Shader.h"

namespace Comp {

const ResId Skybox::smDefaultMaterialId(Skybox::smDefaultAssetName, "Material");

void Skybox::VStaticInit()
{
  Rsl::Asset& defaultAsset = Rsl::GetAsset(smDefaultAssetName);
  defaultAsset.InitFinalize();
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
  Editor::DropResourceWidget<Gfx::Material>(&mMaterialId);
}

} // namespace Comp