#include <math.h>

#include "input.h"
#include "math/constants.h"

#include "camera.h"

Camera::Camera()
{
  mYaw = 0.0f;
  mPitch = 0.0f;
  mPosition = {0.0f, 0.0f, 0.0f};

  mSpeed = 1.0f;
  mSensitivity = 0.5f;

  CalculateBasisVectors();
  mWtc[3][0] = 0.0f;
  mWtc[3][1] = 0.0f;
  mWtc[3][2] = 0.0f;
  mWtc[3][3] = 1.0f;
  CalculateWorldToCamera();
}

void Camera::Update(float dt)
{
  // Change the camera position depending on input.
  if (Input::KeyDown(Input::Key::W))
  {
    mPosition += mForward * dt * mSpeed;
  }
  if (Input::KeyDown(Input::Key::S))
  {
    mPosition -= mForward * dt * mSpeed;
  }
  if (Input::KeyDown(Input::Key::D))
  {
    mPosition += mRight * dt * mSpeed;
  }
  if (Input::KeyDown(Input::Key::A))
  {
    mPosition -= mRight * dt * mSpeed;
  }
  if (Input::KeyDown(Input::Key::E))
  {
    mPosition += mUp * dt * mSpeed;
  }
  if (Input::KeyDown(Input::Key::Q))
  {
    mPosition -= mUp * dt * mSpeed;
  }

  // Change the camera yaw and pitch depending on input and calculate the world
  // to camera transformation.
  if (Input::MouseDown(Input::Mouse::Right))
  {
    Vec2 mouseMotion = Input::MouseMotion();
    mYaw -= mouseMotion[0] * dt * mSensitivity;
    mPitch -= mouseMotion[1] * dt * mSensitivity;
    if (mPitch >= PIO2f)
    {
      mPitch = PIO2f - EPSILONLf;
    } else if (mPitch <= -PIO2f)
    {
      mPitch = -PIO2f + EPSILONLf;
    }
    CalculateBasisVectors();
  }
  CalculateWorldToCamera();
}

const Mat4& Camera::WorldToCamera() const
{
  return mWtc;
}

const Vec3& Camera::Position() const
{
  return mPosition;
}

void Camera::CalculateBasisVectors()
{
  float horizontalScale = cosf(mPitch);
  mForward[0] = cosf(mYaw) * horizontalScale;
  mForward[1] = sinf(mPitch);
  mForward[2] = sinf(mYaw) * horizontalScale;
  Vec3 globalUp = {0.0f, 1.0f, 0.0f};
  mRight = Math::Cross(globalUp, mForward);
  mRight = Math::Normalize(mRight);
  mUp = Math::Cross(mForward, mRight);
}

void Camera::CalculateWorldToCamera()
{
  Vec3 back = -mForward;
  Vec3 negativePosition = -mPosition;
  mWtc[0][0] = mRight[0];
  mWtc[0][1] = mRight[1];
  mWtc[0][2] = mRight[2];
  mWtc[0][3] = Math::Dot(negativePosition, mRight);
  mWtc[1][0] = mUp[0];
  mWtc[1][1] = mUp[1];
  mWtc[1][2] = mUp[2];
  mWtc[1][3] = Math::Dot(negativePosition, mUp);
  mWtc[2][0] = back[0];
  mWtc[2][1] = back[1];
  mWtc[2][2] = back[2];
  mWtc[2][3] = Math::Dot(negativePosition, back);
}
