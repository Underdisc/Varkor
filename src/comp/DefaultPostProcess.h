#ifndef comp_DefaultPostProcess_h
#define comp_DefaultPostProcess_h

#include "AssetLibrary.h"
#include "vlk/Valkor.h"
#include "world/Object.h"

struct DefaultPostProcess
{
  static AssetId smDefaultShaderId;
  AssetId mShaderId;

  void VStaticInit();
  void VInit(const World::Object& owner);
  void VSerialize(Vlk::Value& val);
  void VDeserialize(const Vlk::Explorer& ex);
  void VRender(const World::Object& owner);
  void VEdit(const World::Object& owner);
};

#endif