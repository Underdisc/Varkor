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
  float _yaw;
  float _pitch;
  Vec3 _position;
  // Settings used for the camera controls.
  float _speed;
  float _sensitivity;
  // Values used to create the world to camera transformation.
  Vec3 _forward;
  Vec3 _right;
  Vec3 _up;
  Mat4 _wtc;
};

#endif
