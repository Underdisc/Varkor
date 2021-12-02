#include "gfx/Mesh.h"
#include "math/Vector.h"

namespace Gfx {

size_t AttributeSize(unsigned int attribute)
{
  switch (attribute)
  {
  case Attribute::Position: return sizeof(Vec3);
  case Attribute::Normal: return sizeof(Vec3);
  case Attribute::TexCoord: return sizeof(Vec2);
  }
  return 0;
}

Mesh::Mesh(
  unsigned int attributes,
  size_t vertexByteCount,
  const Ds::Vector<char>& vertexBuffer,
  const Ds::Vector<unsigned int>& elementBuffer):
  mIndexCount(elementBuffer.Size())
{
  // Specify the vertex buffer's attribute layout and send it to the gpu.
  glGenVertexArrays(1, &mVao);
  glBindVertexArray(mVao);
  glGenBuffers(1, &mVbo);
  glBindBuffer(GL_ARRAY_BUFFER, mVbo);
  size_t byteOffset = 0;
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(
    0, 3, GL_FLOAT, GL_FALSE, (GLsizei)vertexByteCount, (void*)byteOffset);
  byteOffset += AttributeSize(Attribute::Position);
  if (attributes & Attribute::Normal)
  {
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
      1, 3, GL_FLOAT, GL_FALSE, (GLsizei)vertexByteCount, (void*)byteOffset);
    byteOffset += AttributeSize(Attribute::Normal);
  }
  if (attributes & Attribute::TexCoord)
  {
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
      2, 2, GL_FLOAT, GL_FALSE, (GLsizei)vertexByteCount, (void*)byteOffset);
    byteOffset += AttributeSize(Attribute::TexCoord);
  }
  glBufferData(
    GL_ARRAY_BUFFER, vertexBuffer.Size(), vertexBuffer.CData(), GL_STATIC_DRAW);

  // Send the element buffer to the gpu.
  glGenBuffers(1, &mEbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    sizeof(unsigned int) * mIndexCount,
    elementBuffer.CData(),
    GL_STATIC_DRAW);

  // Unbind the buffer now that the mesh data has been sent.
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Mesh::Mesh(Mesh&& other):
  mVao(other.mVao),
  mVbo(other.mVbo),
  mEbo(other.mEbo),
  mIndexCount(other.mIndexCount)
{
  other.mVao = 0;
  other.mVbo = 0;
  other.mEbo = 0;
}

Mesh::~Mesh()
{
  glDeleteVertexArrays(1, &mVao);
  glDeleteBuffers(1, &mVbo);
  glDeleteBuffers(1, &mEbo);
}

Mesh& Mesh::operator=(Mesh&& other)
{
  mVao = other.mVao;
  mVbo = other.mVbo;
  mEbo = other.mEbo;
  mIndexCount = other.mIndexCount;

  other.mVao = 0;
  other.mVbo = 0;
  other.mEbo = 0;

  return *this;
}

GLuint Mesh::Vao() const
{
  return mVao;
}

size_t Mesh::IndexCount() const
{
  return mIndexCount;
}

} // namespace Gfx
