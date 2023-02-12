#ifndef editor_InspectorInterface_h
#define editor_InspectorInterface_h

#include "editor/Interface.h"
#include "world/Object.h"

namespace Editor {

struct InspectorInterface: public Interface
{
public:
  InspectorInterface(World::Object& object);
  void Show();
  World::Object mObject;
};

struct AddComponentInterface: public Interface
{
public:
  AddComponentInterface(const World::Object& object);
  void Show();
  World::Object mObject;
};

} // namespace Editor

#endif
