#ifndef editor_Editor_h
#define editor_Editor_h

#include "editor/Camera.h"
#include "editor/CoreInterface.h"
#include "world/Space.h"

namespace Editor {

extern bool nPlayMode;
extern bool nSuppressObjectPicking;
extern bool nHideInterface;
extern World::Space nSpace;
extern CoreInterface nCoreInterface;
extern void (*nExtension)();

void Init();
void Purge();
void StartFrame();
void EndFrame();

} // namespace Editor

#endif
