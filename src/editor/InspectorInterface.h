#ifndef editor_InspectorInterface_h
#define editor_InspectorInterface_h

#include "editor/WindowInterface.h"
#include "world/Object.h"

namespace Editor {

struct InspectorInterface: public WindowInterface
{
public:
  InspectorInterface(World::Object& object);
  void Show();
  World::Object mObject;
};

struct AddComponentInterface: public WindowInterface
{
public:
  AddComponentInterface(const World::Object& object);
  void Show();
  World::Object mObject;
};

} // namespace Editor

#endif
