#include "comp/Relationship.h"

namespace Comp {

void Relationship::VInit(const World::Object& owner) {
  mParent = World::nInvalidMemberId;
}

void Relationship::VSerialize(Vlk::Value& val) {
  val("Parent") = mParent;
  Vlk::Value& childrenVal = val("Children");
  for (MemberId childId: mChildren) {
    childrenVal.PushValue(childId);
  }
}

void Relationship::VDeserialize(const Vlk::Explorer& ex) {
  mParent = ex("Parent").As<MemberId>();
  Vlk::Explorer childrenEx = ex("Children");
  for (int i = 0; i < childrenEx.Size(); ++i) {
    mChildren.Push(childrenEx[i].As<MemberId>());
  }
}

void Relationship::NullifyParent() {
  if (mParent == World::nInvalidMemberId) {
    LogAbort("There is no parent relationship.");
  }
  mParent = World::nInvalidMemberId;
}

void Relationship::NullifyChild(MemberId childId) {
  VResult<size_t> result = mChildren.Find(childId);
  if (!result.Success()) {
    std::stringstream error;
    error << "Child with id \"" << childId << "\" not found.";
    LogAbort(error.str().c_str());
  }
  mChildren.LazyRemove(result.mValue);
}

} // namespace Comp
