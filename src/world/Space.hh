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
void Space::TryRemComponent(MemberId memberId)
{
  TryRemComponent(Comp::Type<T>::smId, memberId);
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
inline T& Space::Add(MemberId memberId)
{
  return AddComponent<T>(memberId);
}

template<typename T>
inline void Space::Rem(MemberId memberId)
{
  RemComponent<T>(memberId);
}

template<typename T>
inline void Space::TryRem(MemberId memberId)
{
  TryRemComponent<T>(memberId);
}

template<typename T>
inline T& Space::Get(MemberId memberId) const
{
  return GetComponent<T>(memberId);
}

template<typename T>
inline T* Space::TryGet(MemberId memberId) const
{
  return TryGetComponent<T>(memberId);
}

template<typename T>
inline bool Space::Has(MemberId memberId) const
{
  return HasComponent<T>(memberId);
}

template<typename T>
Ds::Vector<MemberId> Space::Slice() const
{
  return Slice(Comp::Type<T>::smId);
}

} // namespace World
