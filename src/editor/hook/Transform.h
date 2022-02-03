#ifndef editor_hook_Transform_h
#define editor_hook_Transform_h

#include "comp/Transform.h"
#include "editor/HookInterface.h"
#include "gfx/Framebuffer.h"
#include "math/Geometry.h"

namespace Editor {

template<>
struct Hook<Comp::Transform>: public HookInterface
{
public:
  Hook();
  bool Edit(const World::Object& object);

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

private:
  bool Gizmo(Comp::Transform* transform, const World::Object& object);
  void DisplayOptionsWindow();
  void TryStartOperation(
    Comp::Transform* transform,
    const World::Object& object,
    const Math::Ray& mouseRay);
  bool Translate(
    Comp::Transform* transform,
    const World::Object& object,
    const Math::Ray& mouseRay);
  bool Scale(
    Comp::Transform* transform,
    const World::Object& object,
    const Math::Ray& mouseRay);
  bool Rotate(
    Comp::Transform* transform,
    const World::Object& object,
    const Math::Ray& mouseRay);

  void SwitchMode(Mode newMode);
  Operation GetHandleOperation(World::MemberId handleId) const;
  void PrepareGizmoRepresentation(
    Comp::Transform* transform,
    const World::Object& object,
    Math::Ray* gizmoRay,
    Math::Plane* gizmoPlane,
    Vec3* planeAxis = nullptr) const;
  Quat ReferenceFrameRotation(
    Comp::Transform* transform, const World::Object& object) const;
  Vec3 ScaleToInterval(Vec3 vector, float interval);

  void RenderHandle(World::MemberId handleId, const Vec4& color);
  void RenderHandles(Comp::Transform* transform, const World::Object& object);

private:
  World::Space mSpace;
  Gfx::Framebuffer mDrawbuffer;
  World::MemberId mParent, mX, mY, mZ, mXy, mXz, mYz, mXyz;

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
};

} // namespace Editor

#endif
