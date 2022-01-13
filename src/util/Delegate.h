#ifndef util_Delegate_h
#define util_Delegate_h

#include "Error.h"

namespace Util {

template<typename R, typename... Args>
struct Delegate
{
public:
  bool Open() const
  {
    return mFunction != nullptr;
  }
  void VerifyOpen() const
  {
    LogAbortIf(mFunction == nullptr, "The Delegate is not open.");
  }

  // Closed delegate.
  void BindNull()
  {
    mFunction = nullptr;
  }

  // Free function delegate.
  template<R (*Function)(Args... args)>
  void Bind()
  {
    mFunction = &InternalFreeDelegate<Function>;
  }
  R Invoke(Args... args)
  {
    VerifyOpen();
    return mFunction(nullptr, args...);
  }

  // Member function delegate.
  template<typename T, R (T::*Function)(Args... args)>
  void Bind()
  {
    mFunction = &InternalMemberDelegate<T, Function>;
  }
  R Invoke(void* instance, Args... args) const
  {
    VerifyOpen();
    return mFunction(instance, args...);
  }

private:
  R (*mFunction)(void*, Args... args);

  template<R (*Function)(Args... args)>
  static R InternalFreeDelegate(void* instance, Args... args)
  {
    return Function(args...);
  }
  template<typename T, R (T::*Function)(Args... args)>
  static R InternalMemberDelegate(void* instance, Args... args)
  {
    return (((T*)instance)->*Function)(args...);
  }
};

} // namespace Util

#endif
