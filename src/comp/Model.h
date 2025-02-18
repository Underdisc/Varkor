#ifndef comp_Model_h
#define comp_Model_h

#include "gfx/Renderable.h"
#include "vlk/Valkor.h"
#include "world/Object.h"

namespace Comp {

struct Model {
  void VInit(const World::Object& owner);
  void VSerialize(Vlk::Value& modelVal);
  void VDeserialize(const Vlk::Explorer& modelEx);
  void VRenderable(const World::Object& owner);
  void VEdit(const World::Object& owner);

  ResId mModelId;
  bool mVisible;
};

} // namespace Comp

#endif
