#ifndef editor_Camera_h
#define editor_Camera_h

#include "comp/Camera.h"
#include "comp/Transform.h"
#include "math/Geometry.h"
#include "math/Matrix4.h"
#include "math/Vector.h"

namespace Editor {

class Camera
{
public:
  void Init();
  void Purge();
  World::Object GetObject();
  void Update();

private:
  World::MemberId mCameraId;
  float mYaw;
  float mPitch;

  // Settings used for the camera controls.
  float mSpeed;
  float mSensitivity;
};

} // namespace Editor

#endif
