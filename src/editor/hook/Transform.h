#ifndef editor_hook_Transform_h
#define editor_hook_Transform_h

#include "AssetLibrary.h"
#include "Hook.h"
#include "comp/Transform.h"
#include "gfx/Framebuffer.h"
#include "gfx/Texture.h"
#include "math/Geometry.h"

namespace Editor {
namespace Hook {

template<>
void Edit(Comp::Transform* transform);

template<>
struct Gizmo<Comp::Transform>: public GizmoBase
{
public:
  Gizmo();
  bool Run(Comp::Transform* transform, const World::Object& object);

private:
  enum class Mode
  {
    Translate,
    Scale,
    Rotate
  };
  enum class Operation
  {
    None,
    X,
    Y,
    Z,
    Xy,
    Xz,
    Yz,
    Xyz
  };
  enum class ReferenceFrame
  {
    World,
    Parent,
    Relative,
  };

  void DisplayOptionsWindow();
  void TryStartOperation(
    Comp::Transform* transform,
    const World::Space& space,
    World::MemberId ownerId,
    const Math::Ray& mouseRay);
  bool Translate(
    Comp::Transform* transform,
    const World::Space& space,
    World::MemberId ownerId,
    const Math::Ray& mouseRay);
  bool Scale(
    Comp::Transform* transform,
    const World::Space& space,
    World::MemberId ownerId,
    const Math::Ray& mouseRay);
  bool Rotate(
    Comp::Transform* transform,
    const World::Space& space,
    World::MemberId ownerId,
    const Math::Ray& mouseRay);

  void SwitchMode(Mode newMode);
  Operation GetHandleOperation(World::MemberId handleId) const;
  void PrepareGizmoRepresentation(
    Comp::Transform* transform,
    const World::Space& space,
    World::MemberId ownerId,
    Math::Ray* gizmoRay,
    Math::Plane* gizmoPlane,
    Vec3* planeAxis = nullptr) const;
  Quat ReferenceFrameRotation(
    Comp::Transform* transform,
    const World::Space& space,
    World::MemberId ownerId) const;
  Vec3 ScaleToInterval(Vec3 vector, float interval);

  void RenderHandle(World::MemberId handleId, const Vec4& color);
  void RenderHandles(
    Comp::Transform* transform,
    const World::Space& space,
    World::MemberId ownerId);

  World::Space mSpace;
  World::MemberId mParent, mX, mY, mZ, mXy, mXz, mYz, mXyz;
  Gfx::Framebuffer mDrawbuffer;

  Mode mMode;
  ReferenceFrame mReferenceFrame;
  bool mSnapping;
  float mTranslateSnapInterval;
  float mScaleSnapInterval;
  int mRotationSnapDenominator;

  Operation mOperation;
  Vec3 mStartScale;
  Quat mStartRotation;
  Quat mStartWorldRotation;
  Vec3 mTranslateOffset;

  static bool smRequiredAssetsLoaded;
  static AssetLibrary::AssetId smArrowId;
  static AssetLibrary::AssetId smCubeId;
  static AssetLibrary::AssetId smScaleId;
  static AssetLibrary::AssetId smSphereId;
  static AssetLibrary::AssetId smTorusId;
};

} // namespace Hook
} // namespace Editor

#endif
