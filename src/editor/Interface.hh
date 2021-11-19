#include "comp/Type.h"

namespace Editor {

template<typename T, typename... Args>
T* Interface::OpenInterface(Args&&... args)
{
  // If an instance of the interface exists, we make it known that it's no
  // longer needed and stage a new instance.
  T* interface = FindInterface<T>();
  if (interface != nullptr)
  {
    interface->mOpen = false;
  }
  interface = alloc T(args...);
  std::string interfaceName = Util::GetFullTypename<T>();
  mStagedInterfaces.Push({interfaceName, interface});
  return interface;
}

template<typename T>
void Interface::CloseInterface()
{
  Interface* interface = mInterfaces.Get(Util::GetFullTypename<T>());
  interface->mOpen = false;
}

template<typename T>
T* Interface::FindInterface()
{
  Interface** interface = mInterfaces.Find(Util::GetFullTypename<T>());
  if (interface == nullptr)
  {
    return nullptr;
  }
  return (T*)*interface;
}

} // namespace Editor
