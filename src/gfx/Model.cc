#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <sstream>

#include "Error.h"
#include "Model.h"
#include "util/Memory.h"
#include "util/Utility.h"

namespace Gfx {

Result Model::Init(std::string paths[smInitPathCount])
{
  return Init(paths[0]);
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
  *this = Util::Forward(other);
}

Model& Model::operator=(Model&& other)
{
  mAllDrawInfo = Util::Move(other.mAllDrawInfo);
  mMeshes = Util::Move(other.mMeshes);
  mMaterials = Util::Move(other.mMaterials);
  return *this;
}

Model::~Model()
{
  Purge();
}

Result Model::Init(const std::string& file)
{
  Purge();

  // Import the model and record any errors.
  Assimp::Importer importer;
  unsigned int flags = aiProcess_GenNormals | aiProcess_Triangulate |
    aiProcess_FlipUVs | aiProcess_SortByPType;
  const aiScene* scene = importer.ReadFile(file, flags);
  bool sceneCreated = scene != nullptr && scene->mRootNode != nullptr;
  if (!sceneCreated || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
  {
    std::stringstream error;
    error << "Failed to load " << file << ":" << importer.GetErrorString();
    return Result(error.str());
  } else if (scene->mNumMeshes == 0)
  {
    std::stringstream error;
    error << "Failed to load " << file << ": There was no model data.";
    return Result(error.str());
  }

  // Register all of the nodes in the scene.
  Mat4 parentTransformation;
  Math::Identity(&parentTransformation);
  RegisterNode(*scene, *scene->mRootNode, parentTransformation);

  // Register all of the model's meshes.
  mMeshes.Reserve(scene->mNumMeshes);
  for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
  {
    const aiMesh& assimpMesh = *scene->mMeshes[meshIndex];
    if (assimpMesh.mPrimitiveTypes == (unsigned int)aiPrimitiveType_TRIANGLE)
    {
      RegisterMesh(assimpMesh);
    }
  }

  // Register all of the model's materials.
  std::string fileDir = file.substr(0, file.find_last_of('/') + 1);
  for (unsigned int m = 0; m < scene->mNumMaterials; ++m)
  {
    Result result = RegisterMaterial(*scene->mMaterials[m], fileDir);
    if (!result.Success())
    {
      Purge();
      std::stringstream error;
      error << "Failed to load " << file << ": " << result.mError;
      return Result(error.str());
    }
  }

  AssLib::ModelFInfo fInfo;
  fInfo.mId = AssLib::InitBin<Gfx::Model>::CurrentId();
  fInfo.mMeshIndex = AssLib::ModelFInfo::smInvalidMeshIndex;
  AssLib::AddModelFInfo(fInfo);
  return Result();
}

void Model::FinalizeMesh(const AssLib::ModelFInfo& fInfo)
{
  Mesh& mesh = mMeshes[fInfo.mMeshIndex];
  mesh.Finalize(fInfo.mAttributes, fInfo.mVertexByteCount);
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
  size_t vertexByteCount = AttributeSize(Attribute::Position);
  if (assimpMesh.mNormals != nullptr)
  {
    attributes = attributes | Attribute::Normal;
    vertexByteCount += AttributeSize(Attribute::Normal);
  }
  size_t texCoordCount = 0;
  while (assimpMesh.mTextureCoords[texCoordCount] != nullptr)
  {
    attributes = attributes | Attribute::TexCoord;
    ++texCoordCount;
    vertexByteCount += AttributeSize(Attribute::TexCoord);
  }

  // Create the vertex buffer.
  // The order of the vertex data and Attribute enum values are the same.
  Ds::Vector<char> vertexBuffer;
  vertexBuffer.Resize(vertexByteCount * assimpMesh.mNumVertices);
  size_t byteOffset = 0;
  size_t currentByte = byteOffset;
  // Add positions.
  for (unsigned int v = 0; v < assimpMesh.mNumVertices; ++v)
  {
    const aiVector3D& assimpVertex = assimpMesh.mVertices[v];
    Vec3* vertex = (Vec3*)&vertexBuffer[currentByte];
    (*vertex)[0] = assimpVertex.x;
    (*vertex)[1] = assimpVertex.y;
    (*vertex)[2] = assimpVertex.z;
    currentByte += vertexByteCount;
  }
  byteOffset += AttributeSize(Attribute::Position);
  // Add normals.
  if (attributes & Attribute::Normal)
  {
    currentByte = byteOffset;
    for (unsigned int v = 0; v < assimpMesh.mNumVertices; ++v)
    {
      const aiVector3D& assimpNormal = assimpMesh.mNormals[v];
      Vec3* normal = (Vec3*)&vertexBuffer[currentByte];
      (*normal)[0] = assimpNormal.x;
      (*normal)[1] = assimpNormal.y;
      (*normal)[2] = assimpNormal.z;
      currentByte += vertexByteCount;
    }
    byteOffset += AttributeSize(Attribute::Position);
  }
  // Add texture coordinates.
  if (attributes & Attribute::TexCoord)
  {
    for (size_t t = 0; t < texCoordCount; ++t)
    {
      currentByte = byteOffset + AttributeSize(Attribute::TexCoord) * t;
      for (unsigned int v = 0; v < assimpMesh.mNumVertices; ++v)
      {
        const aiVector3D& assimpTexCoord = assimpMesh.mTextureCoords[t][v];
        Vec2* texCoord = (Vec2*)&vertexBuffer[currentByte];
        (*texCoord)[0] = assimpTexCoord.x;
        (*texCoord)[1] = assimpTexCoord.y;
        currentByte += vertexByteCount;
      }
    }
    byteOffset += AttributeSize(Attribute::TexCoord) * texCoordCount;
  }

  // Create the index buffer.
  const unsigned int indicesPerFace = 3;
  unsigned int indexCount = assimpMesh.mNumFaces * indicesPerFace;
  Ds::Vector<unsigned int> elementBuffer;
  elementBuffer.Resize(indexCount);
  size_t currentIndex = 0;
  for (size_t f = 0; f < assimpMesh.mNumFaces; ++f)
  {
    const aiFace& assimpFace = assimpMesh.mFaces[f];
    for (size_t i = 0; i < indicesPerFace; ++i)
    {
      elementBuffer[currentIndex] = assimpFace.mIndices[i];
      ++currentIndex;
    }
  }

  // Upload the buffer data for the new mesh.
  Mesh newMesh;
  newMesh.Upload(vertexBuffer, elementBuffer);
  mMeshes.Emplace(Util::Move(newMesh));

  // The info needed to finalize one of the meshes on this model is needed by
  // the loader so finalization can be peformed on the main thread.
  AssLib::ModelFInfo fInfo;
  fInfo.mId = AssLib::InitBin<Gfx::Model>::CurrentId();
  fInfo.mMeshIndex = (int)mMeshes.Size() - 1;
  fInfo.mAttributes = attributes;
  fInfo.mVertexByteCount = vertexByteCount;
  AssLib::AddModelFInfo(fInfo);
}

Material::TextureType ConvertAiTextureType(const aiTextureType& type)
{
  switch (type)
  {
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
    for (unsigned int i = 0; i < textureCount; ++i)
    {
      aiString filename;
      if (assimpMat.Get(AI_MATKEY_TEXTURE(type, i), filename) != AI_SUCCESS)
      {
        std::stringstream error;
        error << "Failed to get the filename of texture "
              << TextureTypeToString(type) << "[" << i << "].";
        return Result(error.str());
      }
      std::stringstream fullPath;
      fullPath << fileDir << filename.C_Str();
      Image image;
      Result result = image.Init(fullPath.str());
      if (!result.Success())
      {
        std::stringstream error;
        error << "Failed to initiazlize a texture: " << result.mError;
        return Result(error.str());
      }
      group.mImages.Emplace(Util::Move(image));
    }
    material.mGroups.Emplace(Util::Move(group));
    return Result();
  };

  // Add all of the material's textures and create the material.
  Result result = addTextureGroup(aiTextureType_DIFFUSE);
  if (!result.Success())
  {
    return result;
  }
  result = addTextureGroup(aiTextureType_SPECULAR);
  if (!result.Success())
  {
    return result;
  }
  mMaterials.Emplace(Util::Move(material));
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
  for (unsigned int i = 0; i < assimpNode.mNumMeshes; ++i)
  {
    DrawInfo newDrawInfo;
    newDrawInfo.mTransformation = nodeTransformation;
    newDrawInfo.mMeshIndex = assimpNode.mMeshes[i];
    const aiMesh& assimpMesh = *assimpScene.mMeshes[newDrawInfo.mMeshIndex];
    newDrawInfo.mMaterialIndex = assimpMesh.mMaterialIndex;
    mAllDrawInfo.Push(newDrawInfo);
  }

  for (unsigned int i = 0; i < assimpNode.mNumChildren; ++i)
  {
    RegisterNode(assimpScene, *assimpNode.mChildren[i], nodeTransformation);
  }
}

} // namespace Gfx
