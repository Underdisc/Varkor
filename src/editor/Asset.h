#ifndef editor_Asset_h
#define editor_Asset_h

#include <functional>

#include "AssetLibrary.h"

namespace Editor {

extern bool nShowAssetWindow;
void AssetWindows();
template<typename T>
void SelectAsset(std::function<void(AssetId)> callback);

template<typename T>
struct AssetInfo
{
  // This is used when selecting an Asset.
  static std::function<void(AssetId)> smCallback;
  // This is used for tracking the Asset being edited.
  static AssetId smSelectionId;
};
template<typename T>
std::function<void(AssetId)> AssetInfo<T>::smCallback = nullptr;
template<typename T>
AssetId AssetInfo<T>::smSelectionId = AssLib::nInvalidAssetId;

template<typename T>
void SelectAsset(std::function<void(AssetId)> callback)
{
  AssetInfo<T>::smCallback = callback;
}

} // namespace Editor

#endif
