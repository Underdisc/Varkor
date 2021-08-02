#ifndef comp_Model_h
#define comp_Model_h

#include "AssetLibrary.h"

namespace Vlk {
struct Pair;
struct Explorer;
} // namespace Vlk

namespace Comp {

#pragma pack(push, 1)
struct Model
{
  Model();
  void VSerialize(Vlk::Pair& modelVlk);
  void VDeserialize(const Vlk::Explorer& modelEx);

  AssetLibrary::AssetId mShaderId;
  AssetLibrary::AssetId mModelId;
};
#pragma pack(pop)

} // namespace Comp

#endif
