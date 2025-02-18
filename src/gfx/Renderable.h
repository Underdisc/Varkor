#ifndef gfx_Renderable_h
#define gfx_Renderable_h

#include "ds/Vector.h"
#include "gfx/UniformVector.h"
#include "math/Matrix4.h"
#include "rsl/ResourceId.h"
#include "world/Types.h"

namespace World {
struct Space;
struct Object;
} // namespace World

namespace Gfx {

namespace Renderable {
struct Floater {
  World::MemberId mOwner;
  Mat4 mTransform;
  ResId mMaterialId;
  ResId mMeshId;
  UniformVector mUniforms;
};

struct Skybox {
  World::MemberId mOwner;
  Mat4 mTransform;
  ResId mMaterialId;
  UniformVector mUniforms;
};

struct Icon {
  World::MemberId mOwner;
  Vec3 mTranslation;
  Vec4 mColor;
  ResId mMeshId;
};
} // namespace Renderable

struct Collection {
  Ds::Vector<Renderable::Floater> mFloaters;
  Renderable::Skybox mSkybox;
  Ds::Vector<Renderable::Icon> mIcons;
  UniformVector mUniforms;

  Collection();

  void Collect(const World::Space& space);
  void Collect(const World::Object& object);

  static void Add(Renderable::Floater&& floater);
  static void Use(Renderable::Skybox&& skybox);
  static void Add(Renderable::Icon&& icon);

  void RenderFloaters();
  bool HasSkybox() const;
  void RenderSkybox();
  void RenderIcons(bool memberIds, const World::Object& cameraObject) const;

private:
  static void VerifyActiveCollection();
  static Collection* smActiveCollection;
};

} // namespace Gfx

#endif
