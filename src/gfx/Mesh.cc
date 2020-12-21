#include <glad/glad.h>
#include <sstream>

#include "util/Utility.h"

#include "Mesh.h"

namespace Gfx {

Mesh::Mesh() {}

Mesh::Mesh(
  Ds::Vector<Vertex>&& vertices,
  Ds::Vector<unsigned int>&& indices,
  Ds::Vector<Texture>&& textures):
  mVertices(Util::Move(vertices)),
  mIndices(Util::Move(indices)),
  mTextures(Util::Move(textures))
{
  Upload();
}

Mesh::Mesh(Mesh&& other):
  mVertices(Util::Move(other.mVertices)),
  mIndices(Util::Move(other.mIndices)),
  mTextures(Util::Move(other.mTextures)),
  mVao(other.mVao),
  mVbo(other.mVbo),
  mEbo(other.mEbo)
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
  mVertices = Util::Move(other.mVertices);
  mIndices = Util::Move(other.mIndices);
  mTextures = Util::Move(other.mTextures);
  mVao = other.mVao;
  mVbo = other.mVbo;
  mEbo = other.mEbo;

  other.mVao = 0;
  other.mVbo = 0;
  other.mEbo = 0;

  return *this;
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
  glDrawElements(GL_TRIANGLES, mIndices.Size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void Mesh::Upload()
{
  glGenVertexArrays(1, &mVao);
  glBindVertexArray(mVao);

  // Upload the vertex data to the gpu.
  glGenBuffers(1, &mVbo);
  glBindBuffer(GL_ARRAY_BUFFER, mVbo);
  unsigned int vertexDataSize = sizeof(Vertex) * mVertices.Size();
  glBufferData(
    GL_ARRAY_BUFFER, vertexDataSize, mVertices.CData(), GL_STATIC_DRAW);

  // Upload the index data to the gpu.
  glGenBuffers(1, &mEbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
  unsigned int indexDataSize = sizeof(unsigned int) * mIndices.Size();
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER, indexDataSize, mIndices.CData(), GL_STATIC_DRAW);

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
}

} // namespace Gfx
