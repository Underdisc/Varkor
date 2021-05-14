#ifndef editor_Asset_h
#define editor_Asset_h

#include "AssetLibrary.h"

namespace Editor {

typedef void (*AssetSelectCallback)(AssetLibrary::AssetId, void*);
void StartShaderSelection(AssetSelectCallback callback, void* data = nullptr);
void StartModelSelection(AssetSelectCallback callback, void* data = nullptr);

extern bool nShowAssetWindow;
void ShowAssetWindows();

} // namespace Editor

#endif
