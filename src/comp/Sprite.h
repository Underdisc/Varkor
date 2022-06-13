#ifndef comp_Sprite_h
#define comp_Sprite_h

#include "AssetLibrary.h"
#include "gfx/Shader.h"
#include "vlk/Valkor.h"
#include "world/Object.h"

namespace Comp {

#pragma pack(push, 1)
struct Sprite
{
  void VInit(const World::Object& owner);
  void VSerialize(Vlk::Value& spriteVal);
  void VDeserialize(const Vlk::Explorer& spriteEx);
  void VRender(const World::Object& owner);
  void VEdit(const World::Object& owner);

  struct RenderOptions
  {
    const Gfx::Shader* mShader;
  };
  void Render(const World::Object& owner, const RenderOptions& options) const;

  AssetId mImageId;
  AssetId mShaderId;
};
#pragma pack(pop)

} // namespace Comp

#endif
