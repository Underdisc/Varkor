#ifndef camera_h
#define camera_h

#include "math/matrix.hh"
#include "math/vector.hh"

class Camera
{
public:
  Camera();
  void Update(float dt);
  Mat4 WorldToCamera() const;

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

#endif
