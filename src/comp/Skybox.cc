#include "comp/Skybox.h"
#include "comp/Transform.h"
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

  auto& transform = owner.Get<Transform>();
  Vec3 translation = transform.GetWorldTranslation(owner);
  Vec4 color = {0.0f, 1.0f, 0.0f, 1.0f};
  Gfx::Renderable::Collection::AddIcon(
    {owner.mMemberId, translation, color, "vres/renderer:SkyboxIcon"});
}

void Skybox::VEdit(const World::Object& owner)
{
  Editor::DropResourceIdWidget(Rsl::ResTypeId::Material, &mMaterialId);
}

} // namespace Comp