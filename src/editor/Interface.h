#ifndef editor_Interface_h
#define editor_Interface_h

#include <string>

#include "ds/Map.h"
#include "ds/Vector.h"

namespace Editor {

struct HookInterface;
struct WindowInterface;

struct Interface
{
public:
  Interface();
  virtual ~Interface();

protected:
  // This will track when an interface is no longer needed.
  bool mOpen;

  template<typename T, typename... Args>
  T* OpenInterface(Args&&... args);
  template<typename T>
  void CloseInterface();
  template<typename T>
  T* FindInterface();

private:
  virtual void HandleInterfaces();
  void PurgeInterfaces();

  struct StagedInterface
  {
    std::string mName;
    Interface* mInterface;
  };
  Ds::Map<std::string, Interface*> mInterfaces;
  Ds::Vector<StagedInterface> mStagedInterfaces;

  friend void Run();
  friend void Purge();
  friend HookInterface;
  friend WindowInterface;
};

} // namespace Editor

#include "editor/Interface.hh"

#endif
