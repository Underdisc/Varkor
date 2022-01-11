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

namespace Gfx {

struct Model
{
public:
  static constexpr int smInitPathCount = 1;
  static constexpr const char* smPathNames[smInitPathCount] = {"Model"};
  Result Init(std::string paths[smInitPathCount]);
  void Purge();

  Model();
  Model(Model&& other);
  Model& operator=(Model&& other);
  ~Model();

  Result Init(const std::string& file);
  void FinalizeMesh(const AssLib::ModelFInfo& fInfo);

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

  Ds::Vector<DrawInfo> mAllDrawInfo;
  Ds::Vector<Mesh> mMeshes;
  Ds::Vector<Material> mMaterials;
};

} // namespace Gfx

#endif
