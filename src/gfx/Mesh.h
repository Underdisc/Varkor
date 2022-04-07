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
unsigned int AttributesSize(unsigned int attributes);

struct Mesh
{
  Mesh();
  Mesh(Mesh&& other);
  Mesh& operator=(Mesh&& other);
  ~Mesh();

  void Init(
    unsigned int attributes,
    const Ds::Vector<char>& vertexBuffer,
    const Ds::Vector<unsigned int>& elementBuffer);
  void Init(
    void* vertexBuffer,
    void* elementBuffer,
    unsigned int vertexBufferSize,
    unsigned int elementBufferSize,
    unsigned int attributes,
    unsigned int elementCount);
  void Finalize();

  GLuint Vao() const;
  GLuint Ebo() const;
  size_t IndexCount() const;

private:
  GLuint mVao, mVbo, mEbo;
  size_t mIndexCount;
  unsigned int mAttributes;
};

} // namespace Gfx

#endif
