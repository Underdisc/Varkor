#ifndef editor_Editor_h
#define editor_Editor_h

#include "editor/Camera.h"
#include "editor/CoreInterface.h"

namespace Editor {

extern bool nEditorMode;
extern Camera nCamera;
extern CoreInterface nCoreInterface;

void Init();
void Purge();
void StartFrame();
void EndFrame();

} // namespace Editor

#endif
