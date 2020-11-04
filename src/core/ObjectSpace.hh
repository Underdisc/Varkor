#include "Error.h"

namespace Core {

// todo: Consider requiring every component to manually register with the object
// space it will exist under. This means that the add function will only need to
// verify that the component table exits rather than checking for its existence
// and creating it.
template<typename T>
void ObjectSpace::RegisterComponentType()
{
  ComponentType<T>::Validate();
  int neededSize = ComponentType<T>::smId + 1;
  if (neededSize > mTableLookup.Size())
  {
    int neededElementCount = neededSize - mTableLookup.Size();
    mTableLookup.Push(nInvalidTableRef, neededElementCount);
  }
  LogAbortIf(
    mTableLookup[ComponentType<T>::smId] != nInvalidTableRef,
    "The component type being registered has already been registered with this "
    "object space.");
  RegisterComponentType(ComponentType<T>::smId, sizeof(T));
}

template<typename T>
T& ObjectSpace::AddComponent(ObjRef object)
{
  // Create the component table for the component being added if it doesn't
  // already exist.
  if (!ValidComponentTable(ComponentType<T>::smId))
  {
    RegisterComponentType<T>();
  }
  return *((T*)AddComponent(ComponentType<T>::smId, object));
}

template<typename T>
void ObjectSpace::RemComponent(ObjRef object)
{
  RemComponent(ComponentType<T>::smId, object);
}

template<typename T>
T& ObjectSpace::GetComponent(ObjRef object)
{
  return *((T*)GetComponent(ComponentType<T>::smId, object));
}

template<typename T>
bool ObjectSpace::HasComponent(ObjRef object)
{
  return HasComponent(ComponentType<T>::smId, object);
}

template<typename T>
const T* ObjectSpace::GetComponentData() const
{
  return (T*)GetComponentData(ComponentType<T>::smId);
}

template<typename T>
void ObjectSpace::ShowTable() const
{
  ShowTable(ComponentType<T>::smId);
}

} // namespace Core
