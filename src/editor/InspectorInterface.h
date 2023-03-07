#ifndef editor_InspectorInterface_h
#define editor_InspectorInterface_h

#include "comp/Type.h"
#include "editor/Interface.h"
#include "world/Object.h"

namespace Editor {

struct InspectorInterface: public Interface
{
public:
  InspectorInterface(World::Object& object);
  void Show();
  void ShowGizmos();

  World::Object mObject;
  static Ds::Vector<Comp::TypeId> smOpenTypes;
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
