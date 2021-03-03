#ifndef editor_Primary_h
#define editor_Primary_h

#include "core/Object.h"

namespace Editor {

extern void (*InspectComponents)(const Core::World::Object& selected);
extern void (*AvailableComponents)(const Core::World::Object& selected);

template<typename T>
void InspectComponent(const Core::World::Object& object);
template<typename T>
void MakeComponentAvailable(const Core::World::Object& object);

void Init();
void Start();
void End();

} // namespace Editor

#include "Primary.hh"

#endif
