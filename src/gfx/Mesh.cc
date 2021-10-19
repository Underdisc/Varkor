#include <glad/glad.h>
#include <sstream>

#include "util/Utility.h"

#include "Mesh.h"

namespace Gfx {

Mesh::Mesh(): mTextures(), mVao(0), mVbo(0), mEbo(0), mIndexCount(0) {}

Mesh::Mesh(
  const Ds::Vector<Vertex>& vertices,
  const Ds::Vector<unsigned int>& indices,
  Ds::Vector<Texture>&& textures):
  mTextures(Util::Move(textures))
{
  Upload(vertices, indices);
}

Mesh::Mesh(Mesh&& other):
  mTextures(Util::Move(other.mTextures)),
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
  DeleteBuffers();
}

Mesh& Mesh::operator=(Mesh&& other)
{
  mTextures = Util::Move(other.mTextures);
  mVao = other.mVao;
  mVbo = other.mVbo;
  mEbo = other.mEbo;

  other.mVao = 0;
  other.mVbo = 0;
  other.mEbo = 0;

  return *this;
}

void Mesh::UploadIndices(const Ds::Vector<unsigned int> indices)
{
  glGenBuffers(1, &mEbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    sizeof(unsigned int) * indices.Size(),
    indices.CData(),
    GL_STATIC_DRAW);
  mIndexCount = indices.Size();
}

void Mesh::Upload(
  const Ds::Vector<Vec3>& vertices, const Ds::Vector<unsigned int>& indices)
{
  DeleteBuffers();
  glGenVertexArrays(1, &mVao);
  glBindVertexArray(mVao);

  // Upload the mesh data.
  glGenBuffers(1, &mVbo);
  glBindBuffer(GL_ARRAY_BUFFER, mVbo);
  glBufferData(
    GL_ARRAY_BUFFER,
    sizeof(Vec3) * vertices.Size(),
    vertices.CData(),
    GL_STATIC_DRAW);
  UploadIndices(indices);

  // Specify the layout of the vertex data.
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void*)0);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::Upload(
  const Ds::Vector<Vertex>& vertices, const Ds::Vector<unsigned int>& indices)
{
  DeleteBuffers();
  glGenVertexArrays(1, &mVao);
  glBindVertexArray(mVao);

  // Upload the mesh data.
  glGenBuffers(1, &mVbo);
  glBindBuffer(GL_ARRAY_BUFFER, mVbo);
  size_t vertexDataSize = sizeof(Vertex) * vertices.Size();
  glBufferData(
    GL_ARRAY_BUFFER, vertexDataSize, vertices.CData(), GL_STATIC_DRAW);
  UploadIndices(indices);

  // Specify the layout of the vertex data.
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(
    1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, mNormal));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(
    2,
    2,
    GL_FLOAT,
    GL_FALSE,
    sizeof(Vertex),
    (void*)offsetof(Vertex, mTexCoord));

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::Draw(const Shader& shader) const
{
  unsigned int currentDiffuse = 0;
  unsigned int currentSpecular = 0;

  // Bind all of the necessary textures and set the sampler uniforms to the
  // correct texture ids.
  for (int i = 0; i < mTextures.Size(); ++i)
  {
    std::stringstream uniformName;
    uniformName << "material.";

    const Texture& texture = mTextures[i];
    if (texture.Type() == TextureType::Diffuse)
    {
      uniformName << "diffuseMap[" << currentDiffuse << "]";
      ++currentDiffuse;
    }
    if (texture.Type() == TextureType::Specular)
    {
      uniformName << "specularMap[" << currentSpecular << "]";
      ++currentSpecular;
    }

    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, texture.Id());
    shader.SetSampler(uniformName.str().c_str(), i);
  }
  glActiveTexture(GL_TEXTURE0);

  // Render the mesh.
  glBindVertexArray(mVao);
  glDrawElements(GL_TRIANGLES, (GLsizei)mIndexCount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void Mesh::DeleteBuffers()
{
  glDeleteVertexArrays(1, &mVao);
  glDeleteBuffers(1, &mVbo);
  glDeleteBuffers(1, &mEbo);
}

} // namespace Gfx
