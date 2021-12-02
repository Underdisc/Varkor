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
  Mesh(
    unsigned int attributes,
    size_t vertexByteCount,
    const Ds::Vector<char>& vertexBuffer,
    const Ds::Vector<unsigned int>& elementBuffer);
  Mesh(Mesh&& other);
  ~Mesh();

  Mesh& operator=(Mesh&& other);

  GLuint Vao() const;
  size_t IndexCount() const;

private:
  GLuint mVao, mVbo, mEbo;
  size_t mIndexCount;
};

} // namespace Gfx

#endif
