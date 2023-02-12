#include "comp/Sprite.h"
#include "comp/Transform.h"
#include "editor/Utility.h"
#include "gfx/Image.h"
#include "gfx/Renderer.h"
#include "rsl/Library.h"

namespace Comp {

const ResId Sprite::smDefaultMaterialId(smDefaultAssetName, "Default");

void Sprite::VStaticInit()
{
  Rsl::RequireAsset(smDefaultAssetName);
}

void Sprite::VInit(const World::Object& owner)
{
  mMaterialId = smDefaultMaterialId;
  mScalorImageId = Rsl::GetDefaultResId<Gfx::Image>();
}

void Sprite::VSerialize(Vlk::Value& spriteVal)
{
  spriteVal("MaterialId") = mMaterialId;
  spriteVal("ScalorImageId") = mScalorImageId;
}

void Sprite::VDeserialize(const Vlk::Explorer& spriteEx)
{
  mMaterialId = spriteEx("MaterialId").As<ResId>(smDefaultMaterialId);
  mScalorImageId =
    spriteEx("ScalorImageId").As<ResId>(Rsl::GetDefaultResId<Gfx::Image>());
}

void Sprite::VRenderable(const World::Object& owner)
{
  auto& transform = owner.Get<Comp::Transform>();
  Mat4 aspectScale = GetAspectScale();
  Mat4 transformation = transform.GetWorldMatrix(owner);

  Gfx::Renderable renderable;
  renderable.mOwner = owner.mMemberId;
  renderable.mTransform = transformation * aspectScale;
  renderable.mMaterialId = mMaterialId;
  renderable.mMeshId = Gfx::Renderer::nSpriteMeshId;
  Gfx::Renderable::Collection::Add(
    Gfx::Renderable::Type::Floater, std::move(renderable));
}

void Sprite::VEdit(const World::Object& owner)
{
  Editor::DropResourceIdWidget(Rsl::ResTypeId::Material, &mMaterialId);
  Editor::DropResourceIdWidget(
    Rsl::ResTypeId::Image, &mScalorImageId, "Scalor");
}

Mat4 Sprite::GetAspectScale()
{
  auto* scalorImage = Rsl::TryGetRes<Gfx::Image>(mScalorImageId);
  Mat4 aspectScale;
  if (scalorImage == nullptr) {
    Math::Identity(&aspectScale);
    return aspectScale;
  }
  Vec3 scale = {scalorImage->Aspect(), 1.0f, 1.0f};
  Math::Scale(&aspectScale, scale);
  return aspectScale;
}

} // namespace Comp
