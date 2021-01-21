#include "Types.h"

namespace Core {

int CreateId();

template<typename T>
int ComponentType<T>::smId = nInvalidComponentId;

template<typename T>
void ComponentType<T>::Validate()
{
  if (smId == nInvalidComponentId)
  {
    smId = CreateId();
  }
}

} // namespace Core
