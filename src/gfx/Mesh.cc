#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <imgui/imgui.h>
#include <utility>

#include "editor/Utility.h"
#include "gfx/Mesh.h"
#include "gfx/Model.h"
#include "math/Vector.h"

namespace Gfx {

unsigned int Mesh::AttributesSize(unsigned int attributes) {
  unsigned int size = 0;
  if (attributes & Attribute::Position) {
    size += sizeof(Vec3);
  }
  if (attributes & Attribute::Tangent) {
    size += sizeof(Vec3);
  }
  if (attributes & Attribute::Bitagent) {
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

Mesh::Mesh(Mesh&& other) {
  *this = std::move(other);
}

Mesh& Mesh::operator=(Mesh&& other) {
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

Mesh::~Mesh() {
  Purge();
}

void Mesh::EditConfig(Vlk::Value* configValP) {
  Vlk::Value& configVal = *configValP;
  Vlk::Value& fileVal = configVal("File");
  std::string file = configVal("File").As<std::string>("");
  Editor::DropResourceFileWidget("File", &file);
  fileVal = file;

  Vlk::Value& flipUvsVal = configVal("FlipUvs");
  bool flipUvs = flipUvsVal.As<bool>(false);
  ImGui::Checkbox("Flip Uvs", &flipUvs);
  flipUvsVal = flipUvs;

  Vlk::Value& scaleVal = configVal("Scale");
  float scale = scaleVal.As<float>(1.0f);
  ImGui::PushItemWidth(-Editor::CalcBufferWidth("Scale"));
  ImGui::DragFloat("Scale", &scale, 0.01f);
  ImGui::PopItemWidth();
  scaleVal = scale;
}

Result Mesh::Init() {
  return Result();
}

Result Mesh::Init(const Vlk::Explorer& configEx) {
  Vlk::Explorer fileEx = configEx("File");
  if (!fileEx.Valid(Vlk::Value::Type::TrueValue)) {
    return Result("Missing :File: TrueValue.");
  }
  std::string file = fileEx.As<std::string>();
  const bool flipUvs = configEx("FlipUvs").As<bool>(false);
  float scale = configEx("Scale").As<float>(1.0f);
  return Init(file, flipUvs, scale);
}

Result Mesh::Init(const std::string& file, bool flipUvs, float scale) {
  VResult<Local> result = Local::Init(file, Attribute::All, flipUvs, scale);
  if (!result.Success()) {
    return result;
  }
  return Init(result.mValue);
}

Result Mesh::Init(const aiMesh& assimpMesh, float scale) {
  VResult<Local> result = Local::Init(assimpMesh, Attribute::All, scale);
  if (!result.Success()) {
    return result;
  }
  return Init(result.mValue);
}

VResult<Mesh::Local> Mesh::Local::Init(
  const std::string& file,
  unsigned int selectedAttributes,
  bool flipUvs,
  float scale) {
  Assimp::Importer importer;
  VResult<const aiScene*> importResult =
    Gfx::Model::Import(file, &importer, flipUvs);
  if (!importResult.Success()) {
    return std::move(importResult);
  }
  const aiScene* scene = importResult.mValue;

  if (scene->mNumMeshes != 1) {
    return Result("File \"" + file + "\" can only contain one mesh.");
  }
  return Init(*scene->mMeshes[0], selectedAttributes, scale);
}

VResult<Mesh::Local> Mesh::Local::Init(
  const aiMesh& assimpMesh, unsigned int selectedAttributes, float scale) {
  Local local;
  LogAbortIf(
    !(selectedAttributes & Mesh::Attribute::Position),
    "The position attribute must be selected.");
  // Find the size of a single vertex and all of its attributes.
  local.mAttributes = Mesh::Attribute::Position;
  bool tangentsSelected = selectedAttributes & Mesh::Attribute::Tangent;
  if (tangentsSelected && assimpMesh.mTangents != nullptr) {
    local.mAttributes =
      local.mAttributes | Mesh::Attribute::Tangent | Mesh::Attribute::Bitagent;
  }
  bool normalsSelected = selectedAttributes & Mesh::Attribute::Normal;
  if (normalsSelected && assimpMesh.mNormals != nullptr) {
    local.mAttributes = local.mAttributes | Mesh::Attribute::Normal;
  }
  bool texCoordsSelected = selectedAttributes & Mesh::Attribute::TexCoord;
  if (texCoordsSelected && assimpMesh.mTextureCoords[0] != nullptr) {
    local.mAttributes = local.mAttributes | Mesh::Attribute::TexCoord;
  }
  size_t vertexByteCount = Mesh::AttributesSize(local.mAttributes);

  // Create the vertex buffer. The order of the vertex data and Attribute enum
  // values are the same.
  local.mVertexBuffer.Resize(vertexByteCount * assimpMesh.mNumVertices);
  size_t byteOffset = 0;
  size_t currentByte = byteOffset;
  // Add positions.
  for (unsigned int v = 0; v < assimpMesh.mNumVertices; ++v) {
    const aiVector3D& assimpVertex = assimpMesh.mVertices[v];
    Vec3& vertex = *(Vec3*)&local.mVertexBuffer[currentByte];
    vertex[0] = assimpVertex.x;
    vertex[1] = assimpVertex.y;
    vertex[2] = assimpVertex.z;
    vertex *= scale;
    currentByte += vertexByteCount;
  }
  byteOffset += Mesh::AttributesSize(Mesh::Attribute::Position);
  // Add tangents and bitangents.
  if (local.mAttributes & Mesh::Attribute::Tangent) {
    currentByte = byteOffset;
    for (unsigned int v = 0; v < assimpMesh.mNumVertices; ++v) {
      const aiVector3D& assimpTangent = assimpMesh.mTangents[v];
      const aiVector3D& assimpBitangent = assimpMesh.mBitangents[v];
      Vec3& tangent = *(Vec3*)&local.mVertexBuffer[currentByte];
      Vec3& bitangent =
        *(Vec3*)&local.mVertexBuffer[currentByte + sizeof(Vec3)];
      tangent[0] = assimpTangent.x;
      tangent[1] = assimpTangent.y;
      tangent[2] = assimpTangent.z;
      bitangent[0] = assimpBitangent.x;
      bitangent[1] = assimpBitangent.y;
      bitangent[2] = assimpBitangent.z;
      currentByte += vertexByteCount;
    }
    byteOffset += Mesh::AttributesSize(
      Mesh::Attribute::Tangent | Mesh::Attribute::Bitagent);
  }
  // Add normals.
  if (local.mAttributes & Mesh::Attribute::Normal) {
    currentByte = byteOffset;
    for (unsigned int v = 0; v < assimpMesh.mNumVertices; ++v) {
      const aiVector3D& assimpNormal = assimpMesh.mNormals[v];
      Vec3& normal = *(Vec3*)&local.mVertexBuffer[currentByte];
      normal[0] = assimpNormal.x;
      normal[1] = assimpNormal.y;
      normal[2] = assimpNormal.z;
      currentByte += vertexByteCount;
    }
    byteOffset += Mesh::AttributesSize(Mesh::Attribute::Normal);
  }
  // Add texture coordinates.
  if (local.mAttributes & Mesh::Attribute::TexCoord) {
    currentByte = byteOffset;
    for (unsigned int v = 0; v < assimpMesh.mNumVertices; ++v) {
      const aiVector3D& assimpTexCoord = assimpMesh.mTextureCoords[0][v];
      Vec2& texCoord = *(Vec2*)&local.mVertexBuffer[currentByte];
      texCoord[0] = assimpTexCoord.x;
      texCoord[1] = assimpTexCoord.y;
      currentByte += vertexByteCount;
    }
    byteOffset += Mesh::AttributesSize(Mesh::Attribute::TexCoord);
  }

  // Create the index buffer.
  const unsigned int indicesPerFace = 3;
  unsigned int indexCount = assimpMesh.mNumFaces * indicesPerFace;
  local.mElementBuffer.Resize(indexCount);
  size_t currentIndex = 0;
  for (size_t f = 0; f < assimpMesh.mNumFaces; ++f) {
    const aiFace& assimpFace = assimpMesh.mFaces[f];
    for (size_t i = 0; i < indicesPerFace; ++i) {
      local.mElementBuffer[currentIndex] = assimpFace.mIndices[i];
      ++currentIndex;
    }
  }
  return local;
}

Ds::Vector<Vec3> Mesh::Local::Points() {
  Ds::Vector<Vec3> points;
  for (int b = 0; b < mVertexBuffer.Size(); b += sizeof(Vec3)) {
    points.Push(*(Vec3*)&mVertexBuffer[b]);
  }
  return points;
}

Result Mesh::Init(const Mesh::Local& local) {
  return Init(local.mAttributes, local.mVertexBuffer, local.mElementBuffer);
}

Result Mesh::Init(
  unsigned int attributes,
  const Ds::Vector<Vec3>& vertices,
  const Ds::Vector<unsigned int>& indices) {
  return Init(
    (void*)vertices.CData(),
    3 * sizeof(float) * vertices.Size(),
    attributes,
    (void*)indices.CData(),
    indices.Size() * sizeof(unsigned int),
    indices.Size());
}

Result Mesh::Init(
  unsigned int attributes,
  const Ds::Vector<char>& vertexBuffer,
  const Ds::Vector<unsigned int>& elementBuffer) {
  return Init(
    (void*)vertexBuffer.CData(),
    vertexBuffer.Size(),
    attributes,
    (void*)elementBuffer.CData(),
    sizeof(unsigned int) * elementBuffer.Size(),
    elementBuffer.Size());
}

Result Mesh::Init(
  void* vertexBuffer,
  size_t vertexBufferSize,
  unsigned int attributes,
  void* elementBuffer,
  size_t elementBufferSize,
  size_t elementCount) {
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

void Mesh::Finalize() {
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
  if (mAttributes & Attribute::Tangent) {
    glVertexAttribPointer(
      1, 3, GL_FLOAT, GL_FALSE, vertexByteCount, (void*)byteOffset);
    glEnableVertexAttribArray(1);
    byteOffset += AttributesSize(Attribute::Tangent);
  }
  if (mAttributes & Attribute::Bitagent) {
    glVertexAttribPointer(
      2, 3, GL_FLOAT, GL_FALSE, vertexByteCount, (void*)byteOffset);
    glEnableVertexAttribArray(2);
    byteOffset += AttributesSize(Attribute::Bitagent);
  }
  if (mAttributes & Attribute::Normal) {
    glVertexAttribPointer(
      3, 3, GL_FLOAT, GL_FALSE, vertexByteCount, (void*)byteOffset);
    glEnableVertexAttribArray(3);
    byteOffset += AttributesSize(Attribute::Normal);
  }
  if (mAttributes & Attribute::TexCoord) {
    glVertexAttribPointer(
      4, 2, GL_FLOAT, GL_FALSE, vertexByteCount, (void*)byteOffset);
    glEnableVertexAttribArray(4);
    byteOffset += AttributesSize(Attribute::TexCoord);
  }
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::UpdateVbo(
  size_t byteOffset, size_t byteCount, const void* data) const {
  glBindBuffer(GL_ARRAY_BUFFER, mVbo);
  glBufferSubData(GL_ARRAY_BUFFER, byteOffset, byteCount, data);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::Purge() {
  glDeleteVertexArrays(1, &mVao);
  glDeleteBuffers(1, &mVbo);
  glDeleteBuffers(1, &mEbo);
}

void Mesh::Render() const {
  glBindVertexArray(mVao);
  glDrawElements(GL_TRIANGLES, (GLsizei)mIndexCount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

GLuint Mesh::Vao() const {
  return mVao;
}

GLuint Mesh::Ebo() const {
  return mEbo;
}

bool Mesh::Initialized() const {
  return mVao != 0;
}

} // namespace Gfx
