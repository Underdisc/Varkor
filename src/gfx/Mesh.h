#ifndef gfx_Mesh_h
#define gfx_Mesh_h

#include "ds/Vector.h"
#include "gfx/Shader.h"
#include "gfx/Texture.h"
#include "math/Vector.h"

namespace Gfx {

struct Mesh
{
  struct Vertex
  {
    Vec3 mPosition;
    Vec3 mNormal;
    Vec2 mTexCoord;
  };

  Mesh();
  Mesh(
    const Ds::Vector<Vertex>& vertices,
    const Ds::Vector<unsigned int>& indices,
    Ds::Vector<Texture>&& textures);
  Mesh(Mesh&& other);
  ~Mesh();

  Mesh& operator=(Mesh&& other);

  void Upload(
    const Ds::Vector<Vec3>& vertices, const Ds::Vector<unsigned int>& indices);
  void Upload(
    const Ds::Vector<Vertex>& vertices,
    const Ds::Vector<unsigned int>& indices);
  void Draw(const Shader& shader) const;

private:
  void UploadIndices(const Ds::Vector<unsigned int> indices);
  void DeleteBuffers();

  Ds::Vector<Texture> mTextures;
  unsigned int mVao, mVbo, mEbo;
  size_t mIndexCount;
};

} // namespace Gfx

#endif
