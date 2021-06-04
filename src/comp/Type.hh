#include "editor/hook/Hook.h"
#include "util/Utility.h"

namespace Comp {

// This allows us to know if a specific V prefixed function exists on a type at
// runtime. When used, it will create a function called 'BindVName' where 'Name'
// is the macro's name parameter. If the V prefixed function exists, 'BindVName'
// will bind its delegate parameter to that member function. If it does not, its
// delegate prameter will be bound to null.
// clang-format off
#define Bindable_Type_Function_(name)                                           \
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
  void BindV##name(Util::Delegate* delegate)                                    \
  {                                                                             \
    delegate->Bind<T, &T::V##name>();                                           \
  }                                                                             \
                                                                                \
  template<typename T, typename Util::EnableIf<                                 \
    !HasV##name<T>::smValue, void*>::Type NoFunction = nullptr>                 \
  void BindV##name(Util::Delegate* delegate)                                    \
  {                                                                             \
    delegate->BindNull();                                                       \
  }
// clang-format on
Bindable_Type_Function_(Init);
Bindable_Type_Function_(Update);

template<typename T>
TypeId Type<T>::smId = nInvalidTypeId;

int CreateId();
template<typename T>
void Type<T>::Register()
{
  LogAbortIf(smId != nInvalidTypeId, "Type already registered.");
  smId = CreateId();

  TypeData data;
  data.mName = Util::GetLastName<T>();
  data.mSize = sizeof(T);

  BindVInit<T>(&data.mVInit);
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

} // namespace Comp
