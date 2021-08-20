#ifndef editor_Editor_h
#define editor_Editor_h

#include "editor/Camera.h"

namespace Editor {

extern bool nEditorMode;
extern Camera nCamera;

void Init();
void Purge();
void Run();
void Render();

} // namespace Editor

#endif
