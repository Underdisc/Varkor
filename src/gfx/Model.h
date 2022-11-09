#ifndef gfx_Model_h
#define gfx_Model_h

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <string>

#include "Result.h"
#include "ds/Vector.h"
#include "gfx/Renderable.h"
#include "math/Matrix4.h"
#include "rsl/Asset.h"
#include "rsl/ResourceId.h"

namespace Gfx {

struct Model
{
  Model();
  Model(Model&& other);
  Model& operator=(Model&& other);

  static VResult<const aiScene*> Import(
    const std::string& file, Assimp::Importer* importer, bool flipUvs);

  static void EditConfig(Vlk::Value* configValP);
  static VResult<Model> Init(Rsl::Asset& asset, const Vlk::Explorer& configEx);

  size_t RenderableCount() const;
  Renderable GetRenderable(size_t renderableDescIndex) const;

private:
  struct RenderableDescriptor
  {
    Mat4 mTransformation;
    size_t mMeshIndex;
  };
  struct MeshDescriptor
  {
    ResId mMeshId;
    size_t mMaterialIndex;
  };
  Ds::Vector<RenderableDescriptor> mRenderableDescs;
  Ds::Vector<MeshDescriptor> mMeshDescs;
  Ds::Vector<ResId> mMaterialIds;

  void CreateRenderables(const aiNode& assimpNode, const Mat4& parentTransform);
};

} // namespace Gfx

#endif