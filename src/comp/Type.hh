#include <sstream>

#include "util/Utility.h"

namespace Comp {

// This allows us to know if a specific V prefixed function exists on a type at
// runtime. When used, it will create a function called 'BindVName' where 'Name'
// is the macro's name parameter. If the V prefixed function exists, 'BindVName'
// will bind its delegate parameter to that member function. If it does not, its
// delegate prameter will be bound to null.
// clang-format off
#define BindableTypeFunction(name, ...)                                         \
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
BindableTypeFunction(StaticInit, void);
BindableTypeFunction(Init, void, const World::Object&);
BindableTypeFunction(Update, void, const World::Object&);
BindableTypeFunction(Serialize, void, Vlk::Value&);
BindableTypeFunction(Deserialize, void, const Vlk::Explorer&);
BindableTypeFunction(Render, void, const World::Object&);
BindableTypeFunction(Edit, void);

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
extern Ds::Vector<TypeData> nTypeData;

template<typename T>
template<typename... Dependencies>
void Type<T>::Register()
{
  LogAbortIf(smId != nInvalidTypeId, "Type already registered.");
  smId = CreateId();

  TypeData data;
  data.mName = Util::GetShortTypename<T>();
  data.mSize = sizeof(T);
  data.AddDependencies<T, Dependencies...>();

  data.mDefaultConstruct = &DefaultConstruct<T>;
  data.mCopyConstruct = &CopyConstruct<T>;
  data.mMoveConstruct = &MoveConstruct<T>;
  data.mDestruct = &Destruct<T>;
  BindVStaticInit<T>(&data.mVStaticInit);
  BindVInit<T>(&data.mVInit);
  BindVUpdate<T>(&data.mVUpdate);
  BindVSerialize<T>(&data.mVSerialize);
  BindVDeserialize<T>(&data.mVDeserialize);
  BindVRender<T>(&data.mVRender);
  BindVEdit<T>(&data.mVEdit);

  nTypeData.Push(data);
}

template<typename Dependant, typename Dependency, typename... Rest>
void TypeData::AddDependencies()
{
  if (Type<Dependency>::smId == nInvalidTypeId) {
    std::stringstream error;
    error << "Dependency of " << mName << " (TypeId: " << Type<Dependant>::smId
          << ") not yet Registered.";
    LogAbort(error.str().c_str());
  }
  mDependencies.Push(Type<Dependency>::smId);
  TypeData& dependencyTypeData = nTypeData[Type<Dependency>::smId];
  dependencyTypeData.mDependants.Push(Type<Dependant>::smId);
  AddDependencies<Dependant, Rest...>();
}

template<typename Dependant>
void TypeData::AddDependencies()
{}

template<typename T>
const TypeData& GetTypeData()
{
  LogAbortIf(Type<T>::smId == nInvalidTypeId, "Type not registered.");
  return nTypeData[Type<T>::smId];
}

} // namespace Comp
