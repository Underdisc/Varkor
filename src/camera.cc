#include <math.h>

#include "input.h"
#include "math/constants.h"

#include "camera.h"

Camera::Camera()
{
  _yaw = 0.0f;
  _pitch = 0.0f;
  _position = {0.0f, 0.0f, 0.0f};

  _speed = 1.0f;
  _sensitivity = 0.1f;

  CalculateBasisVectors();
  _wtc[3][0] = 0.0f;
  _wtc[3][1] = 0.0f;
  _wtc[3][2] = 0.0f;
  _wtc[3][3] = 1.0f;
  CalculateWorldToCamera();
}

void Camera::Update(float dt)
{
  // Change the camera position depending on input.
  if (Input::KeyDown(Input::Key::w))
  {
    _position += _forward * dt * _speed;
  }
  if (Input::KeyDown(Input::Key::s))
  {
    _position -= _forward * dt * _speed;
  }
  if (Input::KeyDown(Input::Key::d))
  {
    _position += _right * dt * _speed;
  }
  if (Input::KeyDown(Input::Key::a))
  {
    _position -= _right * dt * _speed;
  }
  if (Input::KeyDown(Input::Key::e))
  {
    _position += _up * dt * _speed;
  }
  if (Input::KeyDown(Input::Key::q))
  {
    _position -= _up * dt * _speed;
  }

  // Change the camera yaw and pitch depending on input and calculate the world
  // to camera transformation.
  if (Input::MouseDown(Input::Mouse::right))
  {
    Vec2 mouse_motion = Input::MouseMotion();
    _yaw -= mouse_motion[0] * dt * _sensitivity;
    _pitch -= mouse_motion[1] * dt * _sensitivity;
    if (_pitch >= PIO2f)
    {
      _pitch = PIO2f - EPSILONLf;
    } else if (_pitch <= -PIO2f)
    {
      _pitch = -PIO2f + EPSILONLf;
    }
    CalculateBasisVectors();
  }
  CalculateWorldToCamera();
}

Mat4 Camera::WorldToCamera() const
{
  return _wtc;
}

void Camera::CalculateBasisVectors()
{
  float horizontal_scale = cosf(_pitch);
  _forward[0] = cosf(_yaw) * horizontal_scale;
  _forward[1] = sinf(_pitch);
  _forward[2] = sinf(_yaw) * horizontal_scale;
  Vec3 global_up = {0.0f, 1.0f, 0.0f};
  _right = Math::Cross(global_up, _forward);
  _right = Math::Normalize(_right);
  _up = Math::Cross(_forward, _right);
}

void Camera::CalculateWorldToCamera()
{
  Vec3 back = -_forward;
  Vec3 negative_position = -_position;
  _wtc[0][0] = _right[0];
  _wtc[0][1] = _right[1];
  _wtc[0][2] = _right[2];
  _wtc[0][3] = Math::Dot(negative_position, _right);
  _wtc[1][0] = _up[0];
  _wtc[1][1] = _up[1];
  _wtc[1][2] = _up[2];
  _wtc[1][3] = Math::Dot(negative_position, _up);
  _wtc[2][0] = back[0];
  _wtc[2][1] = back[1];
  _wtc[2][2] = back[2];
  _wtc[2][3] = Math::Dot(negative_position, back);
}
