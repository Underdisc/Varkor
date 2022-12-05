#ifndef comp_Mesh_h
#define comp_Mesh_h

#include "vlk/Valkor.h"
#include "world/Object.h"

namespace Comp {

#pragma pack(push, 1)
struct Mesh
{
  void VInit(const World::Object& owner);
  void VSerialize(Vlk::Value& modelVal);
  void VDeserialize(const Vlk::Explorer& modelEx);
  void VRenderable(const World::Object& owner);
  void VEdit(const World::Object& owner);

  ResId mMeshId;
  ResId mMaterialId;
  bool mVisible;
};
#pragma pack(pop)

} // namespace Comp

#endif
