#ifndef editor_gizmo_Translator_h
#define editor_gizmo_Translator_h

#include "math/Geometry.h"
#include "math/Quaternion.h"
#include "rsl/ResourceId.h"
#include "world/Types.h"

namespace Editor {
namespace Gizmos {

Vec3 Translate(
  const Vec3& translation,
  const Quat& referenceFrame,
  bool snapping = false,
  float snapInterval = 1.0f);

struct Translator
{
  Translator();
  ~Translator();
  void SetNextOperation(const Vec3& translation, const Quat& referenceFrame);
  Vec3 Run(
    const Vec3& translation,
    const Quat& referenceFrame,
    bool snapping,
    float snapInterval);

  constexpr static const char* smTranslatorAssetName = "vres/translator";
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
  ResId mHandleMaterialIds[smHandleCount];
  Operation mOperation;
  // The vector between the translator's center and the mouse ray's closest
  // point to the translation ray or the mouse ray's intersection with the
  // translation plane.
  Vec3 mMouseOffset;
  // The ray on which translation is performed.
  Math::Ray mTranslationRay;
  // The plane on which translation is performed.
  Math::Plane mTranslationPlane;
};

} // namespace Gizmos
} // namespace Editor

#endif
