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
T* Space::GetComponent(MemberId memberId) const
{
  return (T*)GetComponent(Comp::Type<T>::smId, memberId);
}

template<typename T>
bool Space::HasComponent(MemberId memberId) const
{
  return HasComponent(Comp::Type<T>::smId, memberId);
}

template<typename T>
const T* Space::GetComponentData() const
{
  return (T*)GetComponentData(Comp::Type<T>::smId);
}

template<typename T>
Table::Visitor<T> Space::CreateTableVisitor() const
{
  Table* table = mTables.Find(Comp::Type<T>::smId);
  if (table == nullptr)
  {
    return Table::Visitor<T>(nullptr);
  }
  return table->CreateVisitor<T>();
}

template<typename F>
void Space::VisitActiveMemberIds(F visit) const
{
  for (MemberId i = 0; i < mMembers.Size(); ++i)
  {
    const Member& member = mMembers[i];
    if (member.InUseRootMember())
    {
      visit(i);
    }
  }
}

template<typename F>
void Space::VisitMemberComponentTypeIds(MemberId memberId, F visit) const
{
  VerifyMemberId(memberId);
  const Member& member = mMembers[memberId];
  int currentAddressIndex = member.mAddressIndex;
  while (currentAddressIndex < member.EndAddress())
  {
    const ComponentAddress& address = mAddressBin[currentAddressIndex];
    visit(address.mTypeId);
    ++currentAddressIndex;
  }
}

} // namespace World
