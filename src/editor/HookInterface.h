#ifndef editor_HookInterface_h
#define editor_HookInterface_h

#include "AssetLibrary.h"
#include "editor/Interface.h"
#include "world/Object.h"

namespace Editor {

struct HookInterface: public Interface
{
public:
  virtual ~HookInterface();
  virtual void Edit(const World::Object& object) = 0;

  template<typename T>
  static HookInterface* OpenHookInterface(Interface* interface);
  template<typename T>
  static void CloseHookInterface(Interface* interface);
  template<typename T>
  static HookInterface* FindHookInterface(Interface* interface);

protected:
  template<typename AssetT>
  void SelectAssetWidget(AssetId* id);
};

typedef HookInterface* (*HookOpener)(Interface*);
typedef void (*HookCloser)(Interface*);
typedef HookInterface* (*HookFinder)(Interface*);
struct HookFunctions
{
  HookOpener mOpener;
  HookCloser mCloser;
  HookFinder mFinder;
};

template<typename T>
void RegisterHook();
const HookFunctions& GetHookFunctions(Comp::TypeId id);

template<typename T>
struct Hook: public HookInterface
{
  // The value returned from this function indicates whether the hook should
  // suppress object picking.
  void Edit(const World::Object& object);
};

} // namespace Editor

#include "editor/HookInterface.hh"

#endif
