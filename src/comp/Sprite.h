#ifndef comp_Sprite_h
#define comp_Sprite_h

#include "rsl/ResourceId.h"
#include "vlk/Valkor.h"
#include "world/Object.h"

namespace Comp {

#pragma pack(push, 1)
struct Sprite {
  constexpr static const char* smDefaultAssetName = "vres/sprite";
  static const ResId smDefaultMaterialId;

  void VStaticInit();
  void VInit(const World::Object& owner);
  void VSerialize(Vlk::Value& spriteVal);
  void VDeserialize(const Vlk::Explorer& spriteEx);
  void VRenderable(const World::Object& owner);
  void VEdit(const World::Object& owner);

  Mat4 GetAspectScale();

  ResId mMaterialId;
  ResId mScalorImageId;
};
#pragma pack(pop)

} // namespace Comp

#endif
