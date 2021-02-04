#include "Error.h"

namespace Core {

// todo: Consider requiring every component to manually register with the object
// space it will exist under. This means that the add function will only need to
// verify that the component table exits rather than checking for its existence
// and creating it.
template<typename T>
void Space::RegisterComponentType()
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
    "Space.");
  RegisterComponentType(ComponentType<T>::smId, sizeof(T));
}

template<typename T>
T& Space::AddComponent(MemRef member)
{
  // Create the component table for the component being added if it doesn't
  // already exist.
  if (!ValidTable(ComponentType<T>::smId))
  {
    RegisterComponentType<T>();
  }
  void* memory = AddComponent(ComponentType<T>::smId, member);
  T* compMemory = new (memory) T;
  return *compMemory;
}

template<typename T>
void Space::RemComponent(MemRef member)
{
  RemComponent(ComponentType<T>::smId, member);
}

template<typename T>
T* Space::GetComponent(MemRef member) const
{
  return (T*)GetComponent(ComponentType<T>::smId, member);
}

template<typename T>
bool Space::HasComponent(MemRef member) const
{
  return HasComponent(ComponentType<T>::smId, member);
}

template<typename T>
const T* Space::GetComponentData() const
{
  return (T*)GetComponentData(ComponentType<T>::smId);
}

template<typename T>
Table::Visitor<T> Space::CreateTableVisitor() const
{
  int componentId = ComponentType<T>::smId;
  if (!ValidTable(componentId))
  {
    return Table::Visitor<T>(nullptr);
  }
  TableRef tableRef = mTableLookup[componentId];
  return mTables[tableRef].CreateVisitor<T>();
}

} // namespace Core
