#ifndef CameraOrbiter_h
#define CameraOrbiter_h

#include "math/Vector.h"
#include "world/Object.h"

namespace Comp {

struct CameraOrbiter
{
  Vec3 mPosition;
  float mDistance;
  float mHeight;
  float mRate;

  void VInit(const World::Object& owner);
  void VSerialize(Vlk::Value& val);
  void VDeserialize(const Vlk::Explorer& ex);
  void VUpdate(const World::Object& owner);
  void VEdit(const World::Object& owner);
};

} // namespace Comp

#endif