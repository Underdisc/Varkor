#ifndef editor_Interface_h
#define editor_Interface_h

#include <string>

#include "ds/Map.h"
#include "ds/Vector.h"

namespace Editor {

struct Interface
{
public:
  Interface();
  virtual ~Interface();

  template<typename T, typename... Args>
  T* OpenInterface(Args&&... args);
  template<typename T>
  void CloseInterface();
  template<typename T>
  T* FindInterface();

protected:
  // This will track when an interface is no longer needed.
  bool mOpen;

private:
  void HandleStaging();
  void PurgeInterfaces();
  void ShowAll();
  virtual void Show() = 0;

  struct StagedInterface
  {
    std::string mName;
    Interface* mInterface;
  };
  Ds::Map<std::string, Interface*> mInterfaces;
  Ds::Vector<StagedInterface> mStagedInterfaces;

  friend void EndFrame();
  friend void Purge();
};

} // namespace Editor

#include "editor/Interface.hh"

#endif
