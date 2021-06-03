#ifndef util_Delegate_h
#define util_Delegate_h

namespace Util {

struct Delegate
{
public:
  template<void (*Function)()>
  void Bind();
  template<typename T, void (T::*Function)()>
  void Bind();
  void BindNull();

  void Invoke();
  void Invoke(void* instance);
  bool Open();

private:
  template<void (*Function)()>
  static void InternalFreeFunction(void* instance);
  template<typename T, void (T::*Function)()>
  static void InternalMemberFunction(void* instance);
  void (*mDelegateFunction)(void*);
};

template<void (*Function)()>
static void Delegate::InternalFreeFunction(void* instance)
{
  Function();
}

template<typename T, void (T::*Function)()>
static void Delegate::InternalMemberFunction(void* instance)
{
  (((T*)instance)->*Function)();
}

template<void (*Function)()>
void Delegate::Bind()
{
  mDelegateFunction = InternalFreeFunction<Function>;
}

template<typename T, void (T::*Function)()>
void Delegate::Bind()
{
  mDelegateFunction = InternalMemberFunction<T, Function>;
}

} // namespace Util

#endif
