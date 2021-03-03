#ifndef core_Types_h
#define core_Types_h

namespace World {

typedef signed short TableRef;
typedef TableRef ObjSizeT;
typedef int MemRef;
typedef int SpaceRef;

constexpr int nInvalidComponentId = -1;
constexpr TableRef nInvalidTableRef = -1;
constexpr MemRef nInvalidMemRef = -1;
constexpr SpaceRef nInvalidSpaceRef = -1;

// When a struct is used as a component, a corresponding ComponentType will be
// created and initialized to make an id for that type of component. This is
// necessary for retrieving component data with only type information.
template<typename T>
struct ComponentType
{
  static int smId;
  static void Validate();
};

} // namespace World

#include "Types.hh"

#endif
