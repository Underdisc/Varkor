#ifndef comp_Model_h
#define comp_Model_h

#include "AssetLibrary.h"
#include "vlk/Valkor.h"
#include "world/Object.h"

namespace Comp {

#pragma pack(push, 1)
struct Model
{
  void VInit(const World::Object& owner);
  void VSerialize(Vlk::Value& modelVal);
  void VDeserialize(const Vlk::Explorer& modelEx);
  void VRender(const World::Object& owner);

  AssetLibrary::AssetId mModelId;
  AssetLibrary::AssetId mShaderId;
  bool mVisible;
};
#pragma pack(pop)

} // namespace Comp

#endif
