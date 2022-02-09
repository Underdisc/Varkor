#ifndef editor_Camera_h
#define editor_Camera_h

#include "comp/Camera.h"
#include "comp/Transform.h"
#include "math/Matrix4.h"
#include "math/Vector.h"

namespace Editor {

class Camera
{
public:
  Camera();
  void Update();
  Mat4 View() const;
  const Mat4& InverseView();
  Mat4 Proj() const;

  Vec3 Forward() const;
  Vec3 Right() const;
  Vec3 Up() const;

  const Vec3& Position() const;
  Vec3 StandardToWorldPosition(const Vec2& standardPosition);

private:
  Comp::Transform mTransform;
  Comp::Camera mCamera;
  float mYaw;
  float mPitch;

  // Settings used for the camera controls.
  float mSpeed;
  float mSensitivity;
};

} // namespace Editor

#endif
