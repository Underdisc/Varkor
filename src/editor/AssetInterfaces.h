#ifndef editor_AssetInterface_h
#define editor_AssetInterface_h

#include <functional>

#include "AssetLibrary.h"
#include "editor/WindowInterface.h"

namespace Editor {

template<typename T>
struct AssetInterface: public WindowInterface
{
public:
  AssetInterface();
  void Show();
  void ShowStatus(const AssLib::Asset<T>& asset);
  void EditAssetPaths(AssetId id);
  AssetId mSelectedId;
};

template<typename T>
struct SelectAssetInterface: public WindowInterface
{
public:
  SelectAssetInterface(std::function<void(AssetId)> callback);
  void Show();

private:
  std::function<void(AssetId)> mCallback;
};

} // namespace Editor

#include "editor/AssetInterfaces.hh"

#endif
