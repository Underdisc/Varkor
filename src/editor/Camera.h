#ifndef editor_Camera_h
#define editor_Camera_h

#include "math/Matrix4.h"
#include "math/Vector.h"

namespace Editor {

class Camera
{
public:
  Camera();
  void Update(float dt);
  const Mat4& WorldToCamera() const;
  const Vec3& Position() const;
  const Vec3& Forward() const;

private:
  void CalculateBasisVectors();
  void CalculateWorldToCamera();

  // Values describing the location and orientation of the camera.
  float mYaw;
  float mPitch;
  Vec3 mPosition;

  // Settings used for the camera controls.
  float mSpeed;
  float mSensitivity;

  // Values used to create the world to camera transformation.
  Vec3 mForward;
  Vec3 mRight;
  Vec3 mUp;
  Mat4 mWtc;
};

} // namespace Editor

#endif
