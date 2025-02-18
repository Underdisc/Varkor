#include <sstream>
#include <utility>

#include "util/Memory.h"
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
BindableTypeFunction(Renderable, void, const World::Object&);
BindableTypeFunction(Edit, void, const World::Object&);
BindableTypeFunction(GizmoEdit, void, const World::Object&);

template<typename T>
TypeId Type<T>::smId = nInvalidTypeId;

int CreateId();
extern Ds::Vector<TypeData> nTypeData;

template<typename T>
void Type<T>::Register(const std::string& name) {
  LogAbortIf(smId != nInvalidTypeId, "Type already registered.");
  smId = CreateId();

  // Replace all instances of "::" (scope resolution operator) with "/".
  std::string modifiedName;
  size_t substrStart = 0;
  size_t substrEnd = name.find("::", substrStart);
  while (substrEnd != std::string::npos) {
    modifiedName += name.substr(substrStart, substrEnd - substrStart) + '/';
    substrStart = substrEnd + 2;
    substrEnd = name.find("::", substrStart);
  }
  modifiedName += name.substr(substrStart);

  TypeData data;
  data.mName = std::move(modifiedName);
  data.mSize = sizeof(T);
  data.mDefaultConstruct = &Util::DefaultConstruct<T>;
  data.mCopyConstruct = &Util::CopyConstruct<T>;
  data.mMoveConstruct = &Util::MoveConstruct<T>;
  data.mMoveAssign = &Util::MoveAssign<T>;
  data.mDestruct = &Util::Destruct<T>;
  BindVStaticInit<T>(&data.mVStaticInit);
  BindVInit<T>(&data.mVInit);
  BindVUpdate<T>(&data.mVUpdate);
  BindVSerialize<T>(&data.mVSerialize);
  BindVDeserialize<T>(&data.mVDeserialize);
  BindVRenderable<T>(&data.mVRenderable);
  BindVEdit<T>(&data.mVEdit);
  BindVGizmoEdit<T>(&data.mVGizmoEdit);
  nTypeData.Push(data);

  if (data.mVStaticInit.Open()) {
    data.mVStaticInit.Invoke(nullptr);
  }
}

template<typename T>
template<typename... Dependencies>
void Type<T>::AddDependencies() {
  TypeData& typeData = nTypeData[smId];
  typeData.AddDependencies<T, Dependencies...>();
}

template<typename Dependant, typename Dependency, typename... Rest>
void TypeData::AddDependencies() {
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
void TypeData::AddDependencies() {}

template<typename T>
const TypeData& GetTypeData() {
  LogAbortIf(Type<T>::smId == nInvalidTypeId, "Type not registered.");
  return nTypeData[Type<T>::smId];
}

} // namespace Comp
