#ifndef editor_gizmo_Rotator_h
#define editor_gizmo_Rotator_h

#include "math/Constants.h"
#include "math/Geometry.h"
#include "math/Quaternion.h"
#include "math/Vector.h"
#include "world/Types.h"

namespace Editor::Gizmos {

Quat Rotate(
  const Quat& rotation,
  const Vec3& translation,
  const Quat& referenceFrame,
  bool snapping = false,
  float snapInterval = Math::nPi / 4.0f);

struct Rotator
{
  Rotator();
  ~Rotator();
  void SetNextOperation(const Vec3& translation, const Quat& referenceFrame);
  Quat Run(
    const Quat& rotation,
    const Vec3& translation,
    const Quat& referenceFrame,
    bool snapping,
    float snapInterval);

  constexpr static int smHandleCount = 4;
  constexpr static Vec4 smActiveColor = {1.0f, 1.0f, 1.0f, 1.0f};
  constexpr static Vec4 smHandleColors[] = {
    {0.7f, 0.0f, 0.0f, 1.0f},
    {0.0f, 0.7f, 0.0f, 1.0f},
    {0.0f, 0.0f, 0.7f, 1.0f},
    {0.7f, 0.7f, 0.7f, 0.8f}};

  enum class Operation
  {
    X = 0,
    Y = 1,
    Z = 2,
    Xyz = 3,
    None = 4,
  };

  union
  {
    struct
    {
      World::MemberId mParent;
      World::MemberId mX, mY, mZ, mXyz;
    };
    struct
    {
      World::MemberId mParent;
      World::MemberId mHandles[smHandleCount];
    };
  };
  Operation mOperation;
  // The vector between the rotator's translation and the intersection point of
  // the mouse ray and the rotation plane.
  Vec3 mMouseOffset;
  // The plane whose normal represents the axis of rotation.
  Math::Plane mRotationPlane;
};

} // namespace Editor::Gizmos

#endif