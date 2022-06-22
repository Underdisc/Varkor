#ifndef gfx_Model_h
#define gfx_Model_h

#include <assimp/scene.h>
#include <string>

#include "AssetLibrary.h"
#include "Result.h"
#include "ds/Vector.h"
#include "gfx/Material.h"
#include "gfx/Mesh.h"
#include "math/Matrix4.h"
#include "vlk/Valkor.h"

namespace Gfx {

struct Model
{
public:
  struct InitInfo
  {
    std::string mFile;
    bool mFlipUvs;
    void Serialize(Vlk::Value& val) const;
    void Deserialize(const Vlk::Explorer& ex);
  };
  Result Init(const InitInfo& info);
  void Finalize();
  void Purge();

  Model();
  Model(Model&& other);
  Model& operator=(Model&& other);
  ~Model();

  Result Init(const std::string& file);
  Result Init(
    void* vertexBuffer,
    void* elementBuffer,
    unsigned int vertexBufferSize,
    unsigned int elementBufferSize,
    unsigned int attributes,
    unsigned int elementCount);

  struct DrawInfo
  {
    Mat4 mTransformation;
    unsigned int mMeshIndex;
    unsigned int mMaterialIndex;
  };
  const Ds::Vector<DrawInfo>& GetAllDrawInfo() const;
  const Mesh& GetMesh(unsigned int index) const;
  const Material& GetMaterial(unsigned int index) const;

private:
  void RegisterMesh(const aiMesh& assimpMesh);
  Result RegisterMaterial(
    const aiMaterial& assimpMat, const std::string& fileDir);
  void RegisterNode(
    const aiScene& assimpScene,
    const aiNode& assimpNode,
    const Mat4& parentTransformation);

  Ds::Vector<Mesh> mMeshes;
  Ds::Vector<Material> mMaterials;
  Ds::Vector<DrawInfo> mAllDrawInfo;
};

} // namespace Gfx

#endif
