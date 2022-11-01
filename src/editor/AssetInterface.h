#ifndef editor_AssetsInterface_h
#define editor_AssetsInterface_h

#include <functional>
#include <imgui/imgui.h>

#include "editor/Utility.h"
#include "editor/WindowInterface.h"
#include "rsl/Library.h"
#include "vlk/Valkor.h"

namespace Editor {

struct AssetInterface: public WindowInterface
{
  struct SelectedAsset
  {
    std::string mName;
    Vlk::Value mVal;
  };
  Ds::Vector<SelectedAsset> mSelectedAssets;

  AssetInterface();
  void Show();
  void ShowTabContent(const SelectedAsset& selectedAsset);
  void ShowDefinedResources(const SelectedAsset& selectedAsset);
  void ShowInitializedResources(const Rsl::Asset& asset);

  Result SelectAsset(const std::string& assetName);
  void DeselectAsset(const std::string& assetName);
  bool IsAssetSelected(const std::string& assetName);
};

} // namespace Editor

#endif
