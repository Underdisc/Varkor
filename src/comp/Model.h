#ifndef comp_Model_h
#define comp_Model_h

#include "AssetLibrary.h"

namespace Vlk {
struct Value;
struct Explorer;
} // namespace Vlk

namespace Comp {

#pragma pack(push, 1)
struct Model
{
  void VInit();
  void VSerialize(Vlk::Value& modelVal);
  void VDeserialize(const Vlk::Explorer& modelEx);

  AssetLibrary::AssetId mModelId;
  AssetLibrary::AssetId mShaderId;
};
#pragma pack(pop)

} // namespace Comp

#endif
