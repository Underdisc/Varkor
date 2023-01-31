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

struct Renderable
{
  World::MemberId mOwner;
  Mat4 mTransform;
  ResId mMaterialId;
  ResId mMeshId;
  UniformVector mUniforms;

  void Render(const UniformVector& collectionUniforms) const;

  enum class Type
  {
    Skybox,
    Floater,
    Count,
  };

  struct Collection
  {
    Ds::Vector<Renderable> mRenderables[(int)Type::Count];
    UniformVector mUniforms;

    static Collection* smActiveCollection;
    void Collect(const World::Space& space);
    void Collect(const World::Object& object);
    static void Add(Type renderableType, Renderable&& renderable);

    void Render(Type renderableType) const;
    const Ds::Vector<Renderable>& Get(Type renderableType) const;
  };
};

} // namespace Gfx

#endif