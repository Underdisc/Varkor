#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <filesystem>
#include <sstream>
#include <utility>

#include "AssetLibrary.h"
#include "Error.h"
#include "Model.h"
#include "util/Memory.h"

namespace Gfx {

void Model::InitInfo::Prep(const std::string& file)
{
  mFile = file;
  mFlipUvs = false;
}

void Model::InitInfo::Serialize(Vlk::Value& val) const
{
  val("File") = mFile;
  val("FlipUvs") = mFlipUvs;
}

void Model::InitInfo::Deserialize(const Vlk::Explorer& ex)
{
  mFile = ex("File").As<std::string>("");
  mFlipUvs = ex("FlipUvs").As<bool>(false);
}

void Model::Finalize()
{
  for (Mesh& mesh : mMeshes) {
    mesh.Finalize();
  }
}

void Model::Purge()
{
  mAllDrawInfo.Clear();
  mMeshes.Clear();
  mMaterials.Clear();
}

Model::Model() {}

Model::Model(Model&& other)
{
  *this = std::forward<Model>(other);
}

Model& Model::operator=(Model&& other)
{
  mAllDrawInfo = std::move(other.mAllDrawInfo);
  mMeshes = std::move(other.mMeshes);
  mMaterials = std::move(other.mMaterials);
  return *this;
}

Model::~Model()
{
  Purge();
}

Result Model::Init(const InitInfo& info)
{
  Purge();

  // Resolve the resource path.
  ValueResult<std::string> resolutionResult =
    AssLib::ResolveResourcePath(info.mFile);
  if (!resolutionResult.Success()) {
    return Result(resolutionResult.mError);
  }
  std::string path = resolutionResult.mValue;
  // We perform this check because the importer will cause a crash if the path
  // does not reference a regular file.
  if (!std::filesystem::is_regular_file(path)) {
    std::stringstream error;
    error << "Failed to load \"" << path << "\": Not a regular file.";
    return Result(error.str());
  }

  // Import the model and record any errors.
  Assimp::Importer importer;
  unsigned int flags =
    aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_SortByPType;
  if (info.mFlipUvs) {
    flags |= aiProcess_FlipUVs;
  }
  const aiScene* scene = importer.ReadFile(path, flags);
  bool sceneCreated = scene != nullptr && scene->mRootNode != nullptr;
  if (!sceneCreated || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
    std::stringstream error;
    error << "Failed to load \"" << path << "\": " << importer.GetErrorString();
    return Result(error.str());
  }
  else if (scene->mNumMeshes == 0) {
    std::stringstream error;
    error << "Failed to load \"" << path << "\": There was no model data.";
    return Result(error.str());
  }

  // Register all of the nodes in the scene.
  Mat4 parentTransformation;
  Math::Identity(&parentTransformation);
  RegisterNode(*scene, *scene->mRootNode, parentTransformation);

  // Register all of the model's meshes.
  mMeshes.Reserve(scene->mNumMeshes);
  for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
    const aiMesh& assimpMesh = *scene->mMeshes[meshIndex];
    if (assimpMesh.mPrimitiveTypes & (unsigned int)aiPrimitiveType_TRIANGLE) {
      RegisterMesh(assimpMesh);
    }
  }

  // Register all of the model's materials.
  std::string fileDir = path.substr(0, path.find_last_of('/') + 1);
  for (unsigned int m = 0; m < scene->mNumMaterials; ++m) {
    Result result = RegisterMaterial(*scene->mMaterials[m], fileDir);
    if (!result.Success()) {
      Purge();
      std::stringstream error;
      error << "Failed to load " << path << ": " << result.mError;
      return Result(error.str());
    }
  }
  return Result();
}

Result Model::Init(const std::string& file)
{
  InitInfo info;
  info.mFile = file;
  return Init(info);
}

// Initialize a model with only a single mesh.
Result Model::Init(
  void* vertexBuffer,
  void* elementBuffer,
  unsigned int vertexBufferSize,
  unsigned int elementBufferSize,
  unsigned int attributes,
  unsigned int elementCount)
{
  Purge();

  Mesh newMesh;
  newMesh.Init(
    vertexBuffer,
    elementBuffer,
    vertexBufferSize,
    elementBufferSize,
    attributes,
    elementCount);
  mMeshes.Emplace(std::move(newMesh));

  // The model needs a material so the index on the draw info refers to a valid
  // material. This material, however, is empty.
  mMaterials.Emplace();

  DrawInfo newDrawInfo;
  Math::Identity(&newDrawInfo.mTransformation);
  newDrawInfo.mMeshIndex = 0;
  newDrawInfo.mMaterialIndex = 0;
  mAllDrawInfo.Push(newDrawInfo);

  return Result();
}

const Ds::Vector<Model::DrawInfo>& Model::GetAllDrawInfo() const
{
  return mAllDrawInfo;
}

const Mesh& Model::GetMesh(unsigned int index) const
{
  return mMeshes[index];
}

const Material& Model::GetMaterial(unsigned int index) const
{
  return mMaterials[index];
}

void Model::RegisterMesh(const aiMesh& assimpMesh)
{
  // Find the size of a single vertex and all of its attributes.
  unsigned int attributes = Attribute::Position;
  if (assimpMesh.mNormals != nullptr) {
    attributes = attributes | Attribute::Normal;
  }
  size_t texCoordCount = 0;
  while (assimpMesh.mTextureCoords[texCoordCount] != nullptr) {
    attributes = attributes | Attribute::TexCoord;
    ++texCoordCount;
  }
  size_t vertexByteCount = AttributesSize(attributes);

  // Create the vertex buffer.
  // The order of the vertex data and Attribute enum values are the same.
  Ds::Vector<char> vertexBuffer;
  vertexBuffer.Resize(vertexByteCount * assimpMesh.mNumVertices);
  size_t byteOffset = 0;
  size_t currentByte = byteOffset;
  // Add positions.
  for (unsigned int v = 0; v < assimpMesh.mNumVertices; ++v) {
    const aiVector3D& assimpVertex = assimpMesh.mVertices[v];
    Vec3* vertex = (Vec3*)&vertexBuffer[currentByte];
    (*vertex)[0] = assimpVertex.x;
    (*vertex)[1] = assimpVertex.y;
    (*vertex)[2] = assimpVertex.z;
    currentByte += vertexByteCount;
  }
  byteOffset += AttributesSize(Attribute::Position);
  // Add normals.
  if (attributes & Attribute::Normal) {
    currentByte = byteOffset;
    for (unsigned int v = 0; v < assimpMesh.mNumVertices; ++v) {
      const aiVector3D& assimpNormal = assimpMesh.mNormals[v];
      Vec3* normal = (Vec3*)&vertexBuffer[currentByte];
      (*normal)[0] = assimpNormal.x;
      (*normal)[1] = assimpNormal.y;
      (*normal)[2] = assimpNormal.z;
      currentByte += vertexByteCount;
    }
    byteOffset += AttributesSize(Attribute::Position);
  }
  // Add texture coordinates.
  if (attributes & Attribute::TexCoord) {
    for (size_t t = 0; t < texCoordCount; ++t) {
      currentByte = byteOffset + AttributesSize(Attribute::TexCoord) * t;
      for (unsigned int v = 0; v < assimpMesh.mNumVertices; ++v) {
        const aiVector3D& assimpTexCoord = assimpMesh.mTextureCoords[t][v];
        Vec2* texCoord = (Vec2*)&vertexBuffer[currentByte];
        (*texCoord)[0] = assimpTexCoord.x;
        (*texCoord)[1] = assimpTexCoord.y;
        currentByte += vertexByteCount;
      }
    }
    byteOffset += AttributesSize(Attribute::TexCoord) * texCoordCount;
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

  // Initialize the new mesh.
  Mesh newMesh;
  newMesh.Init(attributes, vertexBuffer, elementBuffer);
  mMeshes.Emplace(std::move(newMesh));
}

Material::TextureType ConvertAiTextureType(const aiTextureType& type)
{
  switch (type) {
  case aiTextureType_DIFFUSE: return Material::TextureType::Diffuse;
  case aiTextureType_SPECULAR: return Material::TextureType::Specular;
  }
  return Material::TextureType::None;
}

Result Model::RegisterMaterial(
  const aiMaterial& assimpMat, const std::string& fileDir)
{
  Material material;

  // This will add a TextureGroup of the desired type to the material.
  auto addTextureGroup = [&](aiTextureType type) -> Result
  {
    Material::TextureGroup group;
    group.mType = ConvertAiTextureType(type);
    unsigned int textureCount = assimpMat.GetTextureCount(type);
    for (unsigned int i = 0; i < textureCount; ++i) {
      aiString filename;
      if (assimpMat.Get(AI_MATKEY_TEXTURE(type, i), filename) != AI_SUCCESS) {
        std::stringstream error;
        error << "Failed to get the filename of texture "
              << aiTextureTypeToString(type) << "[" << i << "].";
        return Result(error.str());
      }
      std::stringstream fullPath;
      fullPath << fileDir << filename.C_Str();
      Image image;
      Result result = image.Init(fullPath.str());
      if (!result.Success()) {
        std::stringstream error;
        error << "Failed to initiazlize a texture: " << result.mError;
        return Result(error.str());
      }
      group.mImages.Emplace(std::move(image));
    }
    material.mGroups.Emplace(std::move(group));
    return Result();
  };

  // Add all of the material's textures and create the material.
  Result result = addTextureGroup(aiTextureType_DIFFUSE);
  if (!result.Success()) {
    return result;
  }
  result = addTextureGroup(aiTextureType_SPECULAR);
  if (!result.Success()) {
    return result;
  }
  mMaterials.Emplace(std::move(material));
  return Result();
}

void Model::RegisterNode(
  const aiScene& assimpScene,
  const aiNode& assimpNode,
  const Mat4& parentTransformation)
{
  // Create all of the DrawInfo instances needed to represent the current node.
  Mat4 nodeTransformation;
  float* assimpTransformStart = (float*)&assimpNode.mTransformation.a1;
  float* nodeTransformStart = nodeTransformation[0];
  constexpr size_t elementCount = sizeof(Mat4) / sizeof(float);
  Util::Copy<float>(assimpTransformStart, nodeTransformStart, elementCount);
  nodeTransformation = parentTransformation * nodeTransformation;
  for (unsigned int i = 0; i < assimpNode.mNumMeshes; ++i) {
    DrawInfo newDrawInfo;
    newDrawInfo.mTransformation = nodeTransformation;
    newDrawInfo.mMeshIndex = assimpNode.mMeshes[i];
    const aiMesh& assimpMesh = *assimpScene.mMeshes[newDrawInfo.mMeshIndex];
    newDrawInfo.mMaterialIndex = assimpMesh.mMaterialIndex;
    mAllDrawInfo.Push(newDrawInfo);
  }

  for (unsigned int i = 0; i < assimpNode.mNumChildren; ++i) {
    RegisterNode(assimpScene, *assimpNode.mChildren[i], nodeTransformation);
  }
}

} // namespace Gfx
