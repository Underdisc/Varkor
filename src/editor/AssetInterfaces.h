#ifndef editor_AssetInterfaces_h
#define editor_AssetInterfaces_h

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
  void ShowStatus(AssLib::Status status);
  void EditInitInfo(AssetId id);
  AssetId mSelectedId;
};

template<typename T>
void DropAssetWidget(AssetId* id);

} // namespace Editor

#include "editor/AssetInterfaces.hh"

#endif
