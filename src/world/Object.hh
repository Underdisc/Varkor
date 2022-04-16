#include "comp/Type.h"

namespace World {

template<typename T>
T& Object::AddComponent() const
{
  return mSpace->AddComponent<T>(mMemberId);
}

template<typename T>
void Object::RemComponent() const
{
  mSpace->RemComponent<T>(mMemberId);
}

template<typename T>
T& Object::GetComponent() const
{
  return mSpace->GetComponent<T>(mMemberId);
}

template<typename T>
T* Object::TryGetComponent() const
{
  return mSpace->TryGetComponent<T>(mMemberId);
}

template<typename T>
bool Object::HasComponent() const
{
  return mSpace->HasComponent<T>(mMemberId);
}

} // namespace World
