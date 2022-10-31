#ifndef editor_gizmo_Scalor_h
#define editor_gizmo_Scalor_h

#include "math/Geometry.h"
#include "math/Quaternion.h"
#include "rsl/ResourceId.h"
#include "world/Types.h"

namespace Editor {
namespace Gizmos {

Vec3 Scale(
  const Vec3& scale,
  const Vec3& translation,
  const Quat& referenceFrame,
  bool snapping = false,
  float snapInterval = 0.5f);

struct Scalor
{
  Scalor();
  ~Scalor();
  void SetNextOperation(
    const Vec3& scale, const Vec3& translation, const Quat& referenceFrame);
  Vec3 Run(
    const Vec3& scale,
    const Vec3& translation,
    const Quat& referenceFrame,
    bool snapping,
    float snapInterval);

  constexpr static const char* smScalorAssetName = "vres/scalor";
  constexpr static int smHandleCount = 7;
  constexpr static Vec4 smActiveColor = {1.0f, 1.0f, 1.0f, 1.0f};
  constexpr static Vec4 smHandleColors[] = {
    {0.7f, 0.0f, 0.0f, 1.0f},
    {0.0f, 0.7f, 0.0f, 1.0f},
    {0.0f, 0.0f, 0.7f, 1.0f},
    {0.7f, 0.7f, 0.0f, 1.0f},
    {0.7f, 0.0f, 0.7f, 1.0f},
    {0.0f, 0.7f, 0.7f, 1.0f},
    {0.7f, 0.7f, 0.7f, 1.0f}};

  enum class Operation
  {
    X = 0,
    Y = 1,
    Z = 2,
    Xy = 3,
    Xz = 4,
    Yz = 5,
    Xyz = 6,
    None = 7,
  };

  World::MemberId mParent;
  ResId mHandleMaterialIds[smHandleCount];
  union
  {
    struct
    {
      World::MemberId mX, mY, mZ, mXy, mXz, mYz, mXyz;
    };
    struct
    {
      World::MemberId mHandles[smHandleCount];
    };
  };
  Operation mOperation;
  // The position of the mouse on the scale ray or scale plane. Whether it's on
  // the ray or plane depends on the operation.
  Vec3 mMousePosition;
  // The ray used to find changes in scale.
  Math::Ray mScaleRay;
  // The plane used to find changes in scale.
  Math::Plane mScalePlane;
  // The vector between the mouse position and the scalor's translation. It's
  // used to track the direction the mouse position must move in to perform
  // uniform scaling.
  Vec3 mUniformScaleDirection;
  // When we perform uniform scaling, the scale argument is normalized to keep
  // track of direction the scale changes in.
  Vec3 mNormalizedStartScale;
};

} // namespace Gizmos
} // namespace Editor

#endif
