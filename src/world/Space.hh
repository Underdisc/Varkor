#include "Error.h"
#include "world/ComponentType.h"

namespace World {

template<typename T>
T& Space::AddComponent(MemberId memberId)
{
  void* memory = AddComponent(ComponentType<T>::smId, memberId);
  T* compMemory = new (memory) T;
  return *compMemory;
}

template<typename T>
void Space::RemComponent(MemberId memberId)
{
  RemComponent(ComponentType<T>::smId, memberId);
}

template<typename T>
T* Space::GetComponent(MemberId memberId) const
{
  return (T*)GetComponent(ComponentType<T>::smId, memberId);
}

template<typename T>
bool Space::HasComponent(MemberId memberId) const
{
  return HasComponent(ComponentType<T>::smId, memberId);
}

template<typename T>
const T* Space::GetComponentData() const
{
  return (T*)GetComponentData(ComponentType<T>::smId);
}

template<typename T>
Table::Visitor<T> Space::CreateTableVisitor() const
{
  Table* table = mTables.Find(ComponentType<T>::smId);
  if (table == nullptr)
  {
    return Table::Visitor<T>(nullptr);
  }
  return table->CreateVisitor<T>();
}

} // namespace World
