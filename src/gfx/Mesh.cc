#include "gfx/Mesh.h"
#include "math/Vector.h"
#include "util/Utility.h"

namespace Gfx {

size_t AttributeSize(unsigned int attribute)
{
  switch (attribute) {
  case Attribute::Position: return sizeof(Vec3);
  case Attribute::Normal: return sizeof(Vec3);
  case Attribute::TexCoord: return sizeof(Vec2);
  }
  return 0;
}

Mesh::Mesh(): mVao(0), mVbo(0), mEbo(0), mIndexCount(0) {}

Mesh::Mesh(Mesh&& other)
{
  *this = Util::Forward(other);
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
  other.mIndexCount = 0;

  return *this;
}

Mesh::~Mesh()
{
  glDeleteVertexArrays(1, &mVao);
  glDeleteBuffers(1, &mVbo);
  glDeleteBuffers(1, &mEbo);
}

void Mesh::Upload(
  const Ds::Vector<char>& vertexBuffer,
  const Ds::Vector<unsigned int>& elementBuffer)
{
  // Upload the vertex buffer.
  glGenBuffers(1, &mVbo);
  glBindBuffer(GL_ARRAY_BUFFER, mVbo);
  glBufferData(
    GL_ARRAY_BUFFER, vertexBuffer.Size(), vertexBuffer.CData(), GL_STATIC_DRAW);

  // Upload the element buffer.
  mIndexCount = elementBuffer.Size();
  glGenBuffers(1, &mEbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    sizeof(unsigned int) * elementBuffer.Size(),
    elementBuffer.CData(),
    GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glFinish();
}

void Mesh::Finalize(unsigned int attributes, size_t vertexByteCount)
{
  // Specify the vertex buffer's attribute layout.
  glGenVertexArrays(1, &mVao);
  glBindVertexArray(mVao);
  glBindBuffer(GL_ARRAY_BUFFER, mVbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
  size_t byteOffset = 0;
  glVertexAttribPointer(
    0, 3, GL_FLOAT, GL_FALSE, (GLsizei)vertexByteCount, (void*)byteOffset);
  glEnableVertexAttribArray(0);
  byteOffset += AttributeSize(Attribute::Position);
  if (attributes & Attribute::Normal) {
    glVertexAttribPointer(
      1, 3, GL_FLOAT, GL_FALSE, (GLsizei)vertexByteCount, (void*)byteOffset);
    glEnableVertexAttribArray(1);
    byteOffset += AttributeSize(Attribute::Normal);
  }
  if (attributes & Attribute::TexCoord) {
    glVertexAttribPointer(
      2, 2, GL_FLOAT, GL_FALSE, (GLsizei)vertexByteCount, (void*)byteOffset);
    glEnableVertexAttribArray(2);
    byteOffset += AttributeSize(Attribute::TexCoord);
  }
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

GLuint Mesh::Vao() const
{
  return mVao;
}

GLuint Mesh::Ebo() const
{
  return mEbo;
}

size_t Mesh::IndexCount() const
{
  return mIndexCount;
}

} // namespace Gfx
