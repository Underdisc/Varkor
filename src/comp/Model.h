#ifndef comp_Model_h
#define comp_Model_h

#include <string>

#include "AssetLibrary.h"

namespace Comp {

#pragma pack(push, 1)
struct Model
{
  Model();
  void EditorHook();

  AssetLibrary::ShaderId mShaderId;
  std::string mAsset;
};
#pragma pack(pop)

} // namespace Comp

#endif
