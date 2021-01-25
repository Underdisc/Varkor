#ifndef gfx_Model_h
#define gfx_Model_h

#include <assimp/scene.h>
#include <string>

#include "ds/Vector.h"
#include "gfx/Mesh.h"
#include "gfx/Shader.h"
#include "gfx/Texture.h"

namespace Gfx {

struct Model
{
  Model(
    const std::string& file,
    bool* success = nullptr,
    std::string* error = nullptr);
  Model(Model&& other);
  void Draw(const Shader& shader) const;

private:
  void ProcessNode(const aiNode* node, const aiScene* scene);
  Mesh ProcessMesh(const aiMesh* mesh, const aiScene* scene);
  void CollectMaterialTextures(
    Ds::Vector<Texture>* textures,
    const aiMaterial* material,
    TextureType type);

  std::string mDirectory;
  Ds::Vector<Mesh> mMeshes;
  Ds::Vector<Texture> mLoadedTextures;
};

} // namespace Gfx

#endif
