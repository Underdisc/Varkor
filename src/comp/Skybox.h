#ifndef comp_Skybox_h
#define comp_Skybox_h

#include "rsl/ResourceId.h"
#include "vlk/Valkor.h"
#include "world/Object.h"

namespace Comp {

#pragma pack(push, 1)
struct Skybox
{
  constexpr static const char* smDefaultAssetName = "vres/skyboxDefaults";
  static const ResId smDefaultMaterialId;

  void VStaticInit();
  void VInit(const World::Object& owner);
  void VSerialize(Vlk::Value& val);
  void VDeserialize(const Vlk::Explorer& ex);
  void VRenderable(const World::Object& owner);
  void VEdit(const World::Object& owner);

  ResId mMaterialId;
};
#pragma pack(pop)

} // namespace Comp

#endif