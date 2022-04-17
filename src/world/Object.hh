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

template<typename T>
inline T& Object::Add() const
{
  return AddComponent<T>();
}

template<typename T>
inline void Object::Rem() const
{
  RemComponent<T>();
}

template<typename T>
inline T& Object::Get() const
{
  return GetComponent<T>();
}

template<typename T>
inline T* Object::TryGet() const
{
  return TryGetComponent<T>();
}

template<typename T>
inline bool Object::Has() const
{
  return HasComponent<T>();
}

} // namespace World
