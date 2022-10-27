#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <imgui/imgui.h>
#include <utility>

#include "editor/Utility.h"
#include "gfx/Mesh.h"
#include "gfx/Model.h"
#include "math/Vector.h"

namespace Gfx {

unsigned int Mesh::AttributesSize(unsigned int attributes)
{
  unsigned int size = 0;
  if (attributes & Attribute::Position) {
    size += sizeof(Vec3);
  }
  if (attributes & Attribute::Normal) {
    size += sizeof(Vec3);
  }
  if (attributes & Attribute::TexCoord) {
    size += sizeof(Vec2);
  }
  return size;
}

Mesh::Mesh(): mVao(0), mVbo(0), mEbo(0), mIndexCount(0), mAttributes(0) {}

Mesh::Mesh(Mesh&& other)
{
  *this = std::move(other);
}

Mesh& Mesh::operator=(Mesh&& other)
{
  mVao = other.mVao;
  mVbo = other.mVbo;
  mEbo = other.mEbo;
  mIndexCount = other.mIndexCount;
  mAttributes = other.mAttributes;

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

void Mesh::EditConfig(Vlk::Value* configValP)
{
  Vlk::Value& configVal = *configValP;
  Vlk::Value& fileVal = configVal("File");
  std::string file = configVal("File").As<std::string>("");
  // todo: drag and drop.
  Editor::InputText("File", &file, -Editor::CalcBufferWidth("File"));
  fileVal = file;

  Vlk::Value& flipUvsVal = configVal("FlipUvs");
  bool flipUvs = flipUvsVal.As<bool>(false);
  ImGui::Checkbox("Flip Uvs", &flipUvs);
  flipUvsVal = flipUvs;
}

Result Mesh::Init(const Vlk::Explorer& configEx)
{
  Vlk::Explorer fileEx = configEx("File");
  if (!fileEx.Valid(Vlk::Value::Type::TrueValue)) {
    return Result("Missing :File: TrueValue.");
  }
  std::string file = fileEx.As<std::string>();
  const bool flipUvs = configEx("FlipUvs").As<bool>(false);
  return Init(file, flipUvs);
}

Result Mesh::Init(const std::string& file, bool flipUvs)
{
  Assimp::Importer importer;
  ValueResult<const aiScene*> importResult =
    Gfx::Model::Import(file, &importer, flipUvs);
  if (!importResult.Success()) {
    return importResult;
  }
  const aiScene* scene = importResult.mValue;

  if (scene->mNumMeshes != 1) {
    return Result("File \"" + file + "\" can only contain one mesh.");
  }
  Init(*scene->mMeshes[0]);
  return Result();
}

Result Mesh::Init(const aiMesh& assimpMesh)
{
  // Find the size of a single vertex and all of its attributes.
  unsigned int attributes = Attribute::Position;
  if (assimpMesh.mNormals != nullptr) {
    attributes = attributes | Attribute::Normal;
  }
  if (assimpMesh.mTextureCoords[0] != nullptr) {
    attributes = attributes | Attribute::TexCoord;
  }
  size_t vertexByteCount = AttributesSize(attributes);

  // Create the vertex buffer. The order of the vertex data and Attribute enum
  // values are the same.
  Ds::Vector<char> vertexBuffer;
  vertexBuffer.Resize(vertexByteCount * assimpMesh.mNumVertices);
  size_t byteOffset = 0;
  size_t currentByte = byteOffset;
  // Add positions.
  for (unsigned int v = 0; v < assimpMesh.mNumVertices; ++v) {
    const aiVector3D& assimpVertex = assimpMesh.mVertices[v];
    Vec3& vertex = *(Vec3*)&vertexBuffer[currentByte];
    vertex[0] = assimpVertex.x;
    vertex[1] = assimpVertex.y;
    vertex[2] = assimpVertex.z;
    currentByte += vertexByteCount;
  }
  byteOffset += AttributesSize(Attribute::Position);
  // Add normals.
  if (attributes & Attribute::Normal) {
    currentByte = byteOffset;
    for (unsigned int v = 0; v < assimpMesh.mNumVertices; ++v) {
      const aiVector3D& assimpNormal = assimpMesh.mNormals[v];
      Vec3& normal = *(Vec3*)&vertexBuffer[currentByte];
      normal[0] = assimpNormal.x;
      normal[1] = assimpNormal.y;
      normal[2] = assimpNormal.z;
      currentByte += vertexByteCount;
    }
    byteOffset += AttributesSize(Attribute::Normal);
  }
  // Add texture coordinates.
  if (attributes & Attribute::TexCoord) {
    currentByte = byteOffset;
    for (unsigned int v = 0; v < assimpMesh.mNumVertices; ++v) {
      const aiVector3D& assimpTexCoord = assimpMesh.mTextureCoords[0][v];
      Vec2& texCoord = *(Vec2*)&vertexBuffer[currentByte];
      texCoord[0] = assimpTexCoord.x;
      texCoord[1] = assimpTexCoord.y;
      currentByte += vertexByteCount;
    }
    byteOffset += AttributesSize(Attribute::TexCoord);
  }

  // Create the index buffer.
  const unsigned int indicesPerFace = 3;
  unsigned int indexCount = assimpMesh.mNumFaces * indicesPerFace;
  Ds::Vector<unsigned int> elementBuffer;
  elementBuffer.Resize(indexCount);
  size_t currentIndex = 0;
  for (size_t f = 0; f < assimpMesh.mNumFaces; ++f) {
    const aiFace& assimpFace = assimpMesh.mFaces[f];
    for (size_t i = 0; i < indicesPerFace; ++i) {
      elementBuffer[currentIndex] = assimpFace.mIndices[i];
      ++currentIndex;
    }
  }
  return Init(attributes, vertexBuffer, elementBuffer);
}

Result Mesh::Init(
  unsigned int attributes,
  const Ds::Vector<char>& vertexBuffer,
  const Ds::Vector<unsigned int>& elementBuffer)
{
  return Init(
    (void*)vertexBuffer.CData(),
    (unsigned int)vertexBuffer.Size(),
    attributes,
    (void*)elementBuffer.CData(),
    (unsigned int)(sizeof(unsigned int) * elementBuffer.Size()),
    (unsigned int)elementBuffer.Size());
}

Result Mesh::Init(
  void* vertexBuffer,
  size_t vertexBufferSize,
  unsigned int attributes,
  void* elementBuffer,
  size_t elementBufferSize,
  size_t elementCount)
{
  mAttributes = attributes;

  // Upload the vertex buffer.
  glGenBuffers(1, &mVbo);
  glBindBuffer(GL_ARRAY_BUFFER, mVbo);
  glBufferData(
    GL_ARRAY_BUFFER,
    (GLsizeiptr)vertexBufferSize,
    vertexBuffer,
    GL_STATIC_DRAW);

  // Upload the element buffer.
  mIndexCount = elementCount;
  glGenBuffers(1, &mEbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    (GLsizeiptr)elementBufferSize,
    elementBuffer,
    GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  return Result();
}

void Mesh::Finalize()
{
  // Specify the vertex buffer's attribute layout.
  glGenVertexArrays(1, &mVao);
  glBindVertexArray(mVao);
  glBindBuffer(GL_ARRAY_BUFFER, mVbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
  GLsizei vertexByteCount = AttributesSize(mAttributes);
  size_t byteOffset = 0;
  if (mAttributes & Attribute::Position) {
    glVertexAttribPointer(
      0, 3, GL_FLOAT, GL_FALSE, vertexByteCount, (void*)byteOffset);
    glEnableVertexAttribArray(0);
    byteOffset += AttributesSize(Attribute::Position);
  }
  if (mAttributes & Attribute::Normal) {
    glVertexAttribPointer(
      1, 3, GL_FLOAT, GL_FALSE, vertexByteCount, (void*)byteOffset);
    glEnableVertexAttribArray(1);
    byteOffset += AttributesSize(Attribute::Normal);
  }
  if (mAttributes & Attribute::TexCoord) {
    glVertexAttribPointer(
      2, 2, GL_FLOAT, GL_FALSE, vertexByteCount, (void*)byteOffset);
    glEnableVertexAttribArray(2);
    byteOffset += AttributesSize(Attribute::TexCoord);
  }
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::Render() const
{
  glBindVertexArray(mVao);
  glDrawElements(GL_TRIANGLES, (GLsizei)mIndexCount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
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
