#include "comp/Type.h"

namespace World {

template<typename T>
void Object::AddComponent() const
{
  AddComponent(Comp::Type<T>::smId);
}

template<typename T>
void Object::RemComponent() const
{
  RemComponent(Comp::Type<T>::smId);
}

template<typename T>
T* Object::GetComponent() const
{
  return (T*)GetComponent(Comp::Type<T>::smId);
}

template<typename T>
bool Object::HasComponent() const
{
  return HasComponent(Comp::Type<T>::smId);
}

} // namespace World
