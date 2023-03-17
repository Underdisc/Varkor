#ifndef editor_gizmo_Rotator_h
#define editor_gizmo_Rotator_h

#include "math/Constants.h"
#include "math/Geometry.h"
#include "math/Quaternion.h"
#include "math/Vector.h"
#include "rsl/ResourceId.h"
#include "world/Types.h"

namespace Editor {
namespace Gizmos {

struct Rotator
{
  Rotator();
  ~Rotator();
  Rotator(Rotator&& other);
  void SetNextOperation(const Vec3& translation, const Quat& referenceFrame);
  Quat Run(
    const Quat& rotation, const Vec3& translation, const Quat& referenceFrame);

  static void Init();
  static void Purge();
  constexpr static const char* smRotatorAssetName = "vres/rotator";
  constexpr static int smHandleCount = 4;
  constexpr static const char* smMaterialNames[] = {"X", "Y", "Z", "Xyz"};
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

  World::MemberId mParent;
  union
  {
    World::MemberId mHandles[smHandleCount];
    struct
    {
      World::MemberId mX, mY, mZ, mXyz;
    };
  };
  Operation mOperation;
  // The vector between the rotator's translation and the intersection point of
  // the mouse ray and the rotation plane.
  Vec3 mMouseOffset;
  // The plane whose normal represents the axis of rotation.
  Math::Plane mRotationPlane;
};

} // namespace Gizmos
} // namespace Editor

#endif