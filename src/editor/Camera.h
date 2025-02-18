#ifndef editor_Camera_h
#define editor_Camera_h

#include "comp/Camera.h"
#include "comp/Transform.h"
#include "editor/Interface.h"
#include "math/Matrix4.h"
#include "math/Vector.h"

namespace Editor {

struct Camera;
extern Camera nCamera;

struct CameraInterface: public Interface {
  void Show();
};

struct Camera {
public:
  void Init();
  void Purge();
  World::Object GetObject();
  void Update();

  // Settings used for the camera controls.
  float mSpeed;
  float mTranslationT;
  float mSensitivity;
  float mRotationT;

private:
  World::MemberId mCameraId;
  Vec3 mTargetTranslation;
  // Yaw and pitch, respectively.
  Vec2 mEulerRotation;
  Vec2 mTargetEulerRotation;
};

} // namespace Editor

#endif
