namespace Editor {

template<typename T>
HookInterface* HookInterface::OpenHookInterface(Interface* interface)
{
  return (HookInterface*)interface->OpenInterface<Hook<T>>();
}

template<typename T>
void HookInterface::CloseHookInterface(Interface* interface)
{
  interface->CloseInterface<Hook<T>>();
}

template<typename T>
HookInterface* HookInterface::FindHookInterface(Interface* interface)
{
  return (HookInterface*)interface->FindInterface<Hook<T>>();
}

extern Ds::Vector<HookFunctions> nHookFunctions;
template<typename T>
void RegisterHook()
{
  LogAbortIf(
    Comp::Type<T>::smId != nHookFunctions.Size(),
    "The smId of the component registering its Hook does not match the current "
    "size of the nHookFunctions vector.");
  HookFunctions hookFunctions;
  hookFunctions.mOpener = HookInterface::OpenHookInterface<T>;
  hookFunctions.mCloser = HookInterface::CloseHookInterface<T>;
  hookFunctions.mFinder = HookInterface::FindHookInterface<T>;
  nHookFunctions.Push(hookFunctions);
}

template<typename T>
bool Hook<T>::Edit(const World::Object& object)
{
  return false;
}

} // namespace Editor
