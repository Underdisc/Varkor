#ifndef editor_Asset_h
#define editor_Asset_h

#include "AssetLibrary.h"

namespace Editor {

typedef void (*ShaderSelectCallback)(AssetLibrary::ShaderId, void*);
void StartShaderSelection(ShaderSelectCallback callback, void* data = nullptr);

extern bool nShowAssetWindow;
void ShowAssetWindows();

} // namespace Editor

#endif
