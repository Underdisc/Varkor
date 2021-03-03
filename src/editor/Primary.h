#ifndef editor_Primary_h
#define editor_Primary_h

#include "core/Object.h"

namespace Editor {

extern void (*InspectComponents)(const World::Object& selected);
extern void (*AvailableComponents)(const World::Object& selected);

template<typename T>
void InspectComponent(const World::Object& object);
template<typename T>
void MakeComponentAvailable(const World::Object& object);

void Init();
void Start();
void End();

} // namespace Editor

#include "Primary.hh"

#endif
