#include "editor/hook/Hook.h"
#include "util/Utility.h"

namespace Comp {

// This allows us to know if a specific V prefixed function exists on a type at
// runtime. When used, it will create a function called 'BindVName' where 'Name'
// is the macro's name parameter. If the V prefixed function exists, 'BindVName'
// will bind its delegate parameter to that member function. If it does not, its
// delegate prameter will be bound to null.
// clang-format off
#define Bindable_Type_Function_(name, ...)                                      \
  template<typename T>                                                          \
  struct HasV##name                                                             \
  {                                                                             \
    typedef char OneChar;                                                       \
    struct TwoChar { char charArray[2]; };                                      \
                                                                                \
    template<typename C>                                                        \
    static OneChar Test(decltype(&C::V##name));                                 \
    template<typename C>                                                        \
    static TwoChar Test(...);                                                   \
    static constexpr bool smValue = sizeof(Test<T>(0)) == sizeof(OneChar);      \
  };                                                                            \
                                                                                \
  template<typename T, typename Util::EnableIf<                                 \
    HasV##name<T>::smValue, decltype(&T::V##name)>::Type Function = &T::V##name>\
  void BindV##name(Util::Delegate<__VA_ARGS__>* delegate)                       \
  {                                                                             \
    delegate->Bind<T, &T::V##name>();                                           \
  }                                                                             \
                                                                                \
  template<typename T, typename Util::EnableIf<                                 \
    !HasV##name<T>::smValue, void*>::Type NoFunction = nullptr>                 \
  void BindV##name(Util::Delegate<__VA_ARGS__>* delegate)                       \
  {                                                                             \
    delegate->BindNull();                                                       \
  }
// clang-format on
Bindable_Type_Function_(Update, void);

template<typename T>
TypeId Type<T>::smId = nInvalidTypeId;

template<typename T>
void DefaultConstruct(void* data)
{
  new (data) T;
}

template<typename T>
void CopyConstruct(void* from, void* to)
{
  new (to) T(*(T*)from);
}

template<typename T>
void MoveConstruct(void* from, void* to)
{
  new (to) T(Util::Move(*(T*)from));
}

template<typename T>
void Destruct(void* data)
{
  (*(T*)data).~T();
}

int CreateId();
template<typename T>
void Type<T>::Register()
{
  LogAbortIf(smId != nInvalidTypeId, "Type already registered.");
  smId = CreateId();

  TypeData data;
  data.mName = Util::GetShortTypename<T>();
  data.mSize = sizeof(T);

  data.mDefaultConstruct = &DefaultConstruct<T>;
  data.mCopyConstruct = &CopyConstruct<T>;
  data.mMoveConstruct = &MoveConstruct<T>;
  data.mDestruct = &Destruct<T>;

  BindVUpdate<T>(&data.mVUpdate);

  data.mEditHook = nullptr;
  data.mGizmoStart = nullptr;
  data.mGizmoRun = nullptr;

  nTypeData.Push(data);
}

template<typename T>
void Type<T>::RegisterEditHook()
{
  LogAbortIf(smId == nInvalidTypeId, "Type has not been registered.");
  TypeData& data = nTypeData[Type<T>::smId];
  data.mEditHook = &Editor::Hook::CastEdit<T>;
}

template<typename T>
void Type<T>::RegisterGizmo()
{
  LogAbortIf(smId == nInvalidTypeId, "Type has not been registered.");
  TypeData& data = nTypeData[Type<T>::smId];
  data.mGizmoStart = &Editor::Hook::Gizmo<T>::Start;
  data.mGizmoRun = &Editor::Hook::GizmoRun<T>;
}

template<typename T>
const TypeData& GetTypeData()
{
  return nTypeData[Type<T>::smId];
}

} // namespace Comp
