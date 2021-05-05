#ifndef editor_hook_Transform_h
#define editor_hook_Transform_h

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
    Math::Plane* gizmoPlane) const;

  void RenderHandle(World::MemberId handleId, const Vec4& color);
  void RenderHandles(
    Comp::Transform* transform,
    const World::Space& space,
    World::MemberId ownerId);

  World::Space mSpace;
  World::MemberId mParent, mX, mY, mZ, mXy, mXz, mYz, mXyz;
  Gfx::Framebuffer mDrawbuffer;
  Mode mMode;

  Operation mOperation;
  Vec3 mStartScale;
  Quat mStartRotation;
  Quat mStartWorldRotation;
  Vec3 mTranslateOffset;

  const char* arrowPath = "vres/model/arrow.fbx";
  const char* cubePath = "vres/model/cube.obj";
  const char* scalePath = "vres/model/scale.fbx";
  const char* spherePath = "vres/model/sphere.fbx";
  const char* torusPath = "vres/model/torus.fbx";
};

} // namespace Hook
} // namespace Editor

#endif
