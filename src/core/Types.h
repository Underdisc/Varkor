#ifndef core_Types_h
#define core_Types_h

namespace Core {

typedef signed short TableRef;
typedef TableRef ObjSizeT;
typedef int MemRef;
typedef int SpaceRef;

constexpr int nInvalidComponentId = -1;
constexpr TableRef nInvalidTableRef = -1;
constexpr MemRef nInvalidMemRef = -1;
constexpr SpaceRef nInvalidSpaceRef = -1;

// Though it is accessible, nComponentCount should not be modified anywhere
// but this file. It is used to hand out component ids, and only the system
// responsible for handing out component ids should change it.
extern int nComponentCount;

// When a struct is used as a component, a corresponding ComponentType will be
// created and initialized to make an id for that type of component. This is
// necessary for retrieving component data with only type information.
template<typename T>
struct ComponentType
{
  static int smId;
  static void Init();
  static void Validate();
};

template<typename T>
int ComponentType<T>::smId = nInvalidComponentId;

template<typename T>
void ComponentType<T>::Init()
{
  smId = nComponentCount;
  ++nComponentCount;
}

template<typename T>
void ComponentType<T>::Validate()
{
  if (smId == nInvalidComponentId)
  {
    Init();
  }
}

} // namespace Core

// This is the only part of this file that should be used outside of the Core
// namespace. All other declarations and definitions are intended to be used
// within the Core namespace only.
typedef Core::MemRef MemRef;

#endif
