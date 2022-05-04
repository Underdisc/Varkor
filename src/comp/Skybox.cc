#include "comp/Skybox.h"
#include "gfx/Cubemap.h"
#include "gfx/Shader.h"

namespace Comp {

void Skybox::VInit(const World::Object& owner)
{
  mCubemapId = AssLib::nDefaultAssetId;
  mShaderId = AssLib::nDefaultSkyboxShaderId;
}

void Skybox::VSerialize(Vlk::Value& val)
{
  val("CubemapId") = mCubemapId;
  val("ShaderId") = mShaderId;
}

void Skybox::VDeserialize(const Vlk::Explorer& ex)
{
  mCubemapId = ex("CubemapId").As<AssetId>(AssLib::nDefaultAssetId);
  mShaderId = ex("ShaderId").As<AssetId>(AssLib::nDefaultSkyboxShaderId);
}

} // namespace Comp

namespace Editor {

void Hook<Comp::Skybox>::Edit(const World::Object& object)
{
  auto& skybox = object.Get<Comp::Skybox>();
  SelectAssetWidget<Gfx::Cubemap>(&skybox.mCubemapId);
  SelectAssetWidget<Gfx::Shader>(&skybox.mShaderId);
}

} // namespace Editor
