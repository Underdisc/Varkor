#ifndef world_Types_h
#define world_Types_h

namespace World {

typedef signed short TableId;
typedef TableId ObjSizeT;
typedef int MemberId;
typedef int SpaceId;

constexpr int nInvalidComponentId = -1;
constexpr TableId nInvalidTableId = -1;
constexpr MemberId nInvalidMemberId = -1;
constexpr SpaceId nInvalidSpaceId = -1;

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
