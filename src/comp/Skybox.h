#ifndef comp_Skybox_h
#define comp_Skybox_h

#include "AssetLibrary.h"
#include "editor/HookInterface.h"
#include "vlk/Valkor.h"
#include "world/Object.h"

namespace Comp {

#pragma pack(push, 1)
struct Skybox
{
  void VInit(const World::Object& owner);
  void VSerialize(Vlk::Value& val);
  void VDeserialize(const Vlk::Explorer& ex);

  AssetId mCubemapId;
  AssetId mShaderId;
};
#pragma pack(pop)

} // namespace Comp

namespace Editor {

template<>
struct Hook<Comp::Skybox>: public HookInterface
{
  void Edit(const World::Object& object);
};

} // namespace Editor

#endif