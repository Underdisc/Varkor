#ifndef editor_Primary_h
#define editor_Primary_h

#include "editor/Camera.h"
#include "world/Object.h"

namespace Editor {

extern void (*InspectComponents)(const World::Object& selected);
extern void (*AvailableComponents)(const World::Object& selected);

void Init();
void Purge();
void Start();
void Show();
void End();
bool EditorMode();
const Camera& GetCamera();

} // namespace Editor

#endif
