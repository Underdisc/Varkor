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
    Ds::Vector<Vertex>&& vertices,
    Ds::Vector<unsigned int>&& indices,
    Ds::Vector<Texture>&& textures);
  Mesh(Mesh&& other);
  ~Mesh();

  Mesh& operator=(Mesh&& other);

  void Draw(const Shader& shader) const;

private:
  void Upload();

  Ds::Vector<Vertex> mVertices;
  Ds::Vector<unsigned int> mIndices;
  Ds::Vector<Texture> mTextures;
  unsigned int mVao, mVbo, mEbo;
};

} // namespace Gfx

#endif
