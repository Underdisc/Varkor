#ifndef gfx_Mesh_h
#define gfx_Mesh_h

#include <glad/glad.h>

#include "ds/Vector.h"

namespace Gfx {

enum Attribute : unsigned int
{
  Position = 1 << 0,
  Normal = 1 << 1,
  TexCoord = 1 << 2,
};
size_t AttributeSize(unsigned int attribute);

struct Mesh
{
  Mesh();
  Mesh(Mesh&& other);
  Mesh& operator=(Mesh&& other);
  ~Mesh();

  void Upload(
    const Ds::Vector<char>& vertexBuffer,
    const Ds::Vector<unsigned int>& elementBuffer);
  void Finalize(unsigned int attributes, size_t vertexByteCount);

  GLuint Vao() const;
  GLuint Ebo() const;
  size_t IndexCount() const;

private:
  GLuint mVao, mVbo, mEbo;
  size_t mIndexCount;
};

} // namespace Gfx

#endif
