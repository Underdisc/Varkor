#ifndef comp_Relationship_h
#define comp_Relationship_h

#include <string>

#include "vlk/Valkor.h"
#include "world/Object.h"
#include "world/Types.h"

namespace Comp {

#pragma pack(push, 1)
struct Relationship {
  void VInit(const World::Object& owner);
  void VSerialize(Vlk::Value& nameVal);
  void VDeserialize(const Vlk::Explorer& nameEx);

  void NullifyParent();
  void NullifyChild(MemberId childId);

  inline bool HasParent() const;
  inline bool HasChildren() const;
  inline bool HasRelationship() const;

  MemberId mParent;
  Ds::Vector<MemberId> mChildren;
};
#pragma pack(pop)

inline bool Relationship::HasParent() const {
  return mParent != World::nInvalidMemberId;
}

inline bool Relationship::HasChildren() const {
  return !mChildren.Empty();
}

inline bool Relationship::HasRelationship() const {
  return HasParent() || HasChildren();
}

} // namespace Comp

#endif
