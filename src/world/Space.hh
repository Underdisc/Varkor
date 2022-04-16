#include "Error.h"
#include "comp/Type.h"

namespace World {

template<typename T>
T& Space::AddComponent(MemberId memberId)
{
  return *(T*)AddComponent(Comp::Type<T>::smId, memberId);
}

template<typename T>
void Space::RemComponent(MemberId memberId)
{
  RemComponent(Comp::Type<T>::smId, memberId);
}

template<typename T>
T& Space::GetComponent(MemberId memberId) const
{
  return *(T*)GetComponent(Comp::Type<T>::smId, memberId);
}

template<typename T>
T* Space::TryGetComponent(MemberId memberId) const
{
  return (T*)TryGetComponent(Comp::Type<T>::smId, memberId);
}

template<typename T>
bool Space::HasComponent(MemberId memberId) const
{
  return HasComponent(Comp::Type<T>::smId, memberId);
}

template<typename T>
void Space::VisitTableComponents(
  std::function<void(World::MemberId, T&)> visit) const
{
  Table* table = mTables.Find(Comp::Type<T>::smId);
  if (table == nullptr) {
    return;
  }
  table->VisitActiveIndices(
    [&table, &visit](size_t index)
    {
      MemberId owner = table->GetOwner(index);
      visit(owner, *(T*)table->GetComponent(index));
    });
}

} // namespace World
