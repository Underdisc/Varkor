#include "comp/Skybox.h"
#include "comp/Transform.h"
#include "editor/Utility.h"
#include "gfx/Renderer.h"
#include "rsl/Library.h"

namespace Comp {

const ResId Skybox::smDefaultMaterialId(Skybox::smDefaultAssetName, "Default");

void Skybox::VStaticInit() {
  Rsl::RequireAsset(smDefaultAssetName);
}

void Skybox::VInit(const World::Object& owner) {
  mMaterialId = smDefaultMaterialId;
}

void Skybox::VSerialize(Vlk::Value& val) {
  val("MaterialId") = mMaterialId;
}

void Skybox::VDeserialize(const Vlk::Explorer& ex) {
  mMaterialId = ex("MaterialId").As<ResId>(smDefaultMaterialId);
}

void Skybox::VRenderable(const World::Object& owner) {
  Gfx::Renderable::Skybox skybox;
  skybox.mOwner = owner.mMemberId;
  skybox.mMaterialId = mMaterialId;
  Gfx::Collection::Use(std::move(skybox));

  Gfx::Renderable::Icon icon;
  icon.mOwner = owner.mMemberId;
  auto& transform = owner.Get<Transform>();
  icon.mTranslation = transform.GetWorldTranslation(owner);
  icon.mColor = {0.0f, 1.0f, 0.0f, 1.0f};
  icon.mMeshId = "vres/renderer:SkyboxIcon";
  Gfx::Collection::Add(std::move(icon));
}

void Skybox::VEdit(const World::Object& owner) {
  Editor::DropResourceIdWidget(Rsl::ResTypeId::Material, &mMaterialId);
}

} // namespace Comp
