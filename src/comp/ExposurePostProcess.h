#ifndef comp_ExposurePostProcess_h
#define comp_ExposurePostProcess_h

#include "AssetLibrary.h"
#include "vlk/Valkor.h"
#include "world/Object.h"

struct ExposurePostProcess
{
  static AssetId smShaderId;

  float mExposure;
  constexpr static float smDefaultExposure = 1.0f;

  void VStaticInit();
  void VInit(const World::Object& owner);
  void VSerialize(Vlk::Value& val);
  void VDeserialize(const Vlk::Explorer& ex);
  void VRender(const World::Object& owner);
  void VEdit(const World::Object& owner);
};

#endif