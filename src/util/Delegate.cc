#include "util/Delegate.h"

namespace Util {

void Delegate::BindNull()
{
  mDelegateFunction = nullptr;
}

void Delegate::Invoke()
{
  mDelegateFunction(nullptr);
}

void Delegate::Invoke(void* instance)
{
  mDelegateFunction(instance);
}

bool Delegate::Open()
{
  return mDelegateFunction != nullptr;
}

} // namespace Util
