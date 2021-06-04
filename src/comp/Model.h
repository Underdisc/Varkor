#ifndef comp_Model_h
#define comp_Model_h

#include "AssetLibrary.h"

namespace Comp {

#pragma pack(push, 1)
struct Model
{
  void VInit();

  AssetLibrary::AssetId mShaderId;
  AssetLibrary::AssetId mModelId;
};
#pragma pack(pop)

} // namespace Comp

#endif
