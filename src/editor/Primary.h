#ifndef editor_Primary_h
#define editor_Primary_h

#include "editor/Camera.h"
#include "world/Object.h"

namespace Editor {

extern void (*InspectComponents)(const World::Object& selected);
extern void (*AvailableComponents)(const World::Object& selected);

template<typename T>
void InspectComponent(const World::Object& object);
template<typename T>
void MakeComponentAvailable(const World::Object& object);

void Init();
void Start();
void Show();
void End();
bool EditorMode();
const Camera& GetCamera();

} // namespace Editor

#include "Primary.hh"

#endif
