#ifndef comp_Skybox_h
#define comp_Skybox_h

#include "AssetLibrary.h"
#include "vlk/Valkor.h"
#include "world/Object.h"

namespace Comp {

#pragma pack(push, 1)
struct Skybox
{
  void VInit(const World::Object& owner);
  void VSerialize(Vlk::Value& val);
  void VDeserialize(const Vlk::Explorer& ex);
  void VRender(const World::Object& owner);
  void VEdit(const World::Object& owner);

  AssetId mCubemapId;
  AssetId mShaderId;
};
#pragma pack(pop)

} // namespace Comp

#endif