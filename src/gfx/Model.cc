#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <sstream>

#include "Error.h"
#include "util/Utility.h"

#include "Model.h"

namespace Gfx {

Model::Model(const std::string& file, bool* success, std::string* error):
  mDirectory(file.substr(0, file.find_last_of('/') + 1))
{
  // Import the model file.
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(
    file, aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_FlipUVs);

  // Record any errors that may have occurred while importing the file.
  bool sceneCreated = scene != nullptr && scene->mRootNode != nullptr;
  std::stringstream errorStream;
  if (!sceneCreated || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
  {
    errorStream << "Assimp failed to load " << file << "." << std::endl;
    errorStream << importer.GetErrorString();
  } else if (scene->mNumMeshes == 0)
  {
    errorStream << "There was no model data in " << file << ".";
  }

  // Avoid processing the imported data if any errors occured.
  if (!errorStream.str().empty())
  {
    LogAbortIf(
      success == nullptr || error == nullptr, errorStream.str().c_str());
    *success = false;
    *error = errorStream.str();
    return;
  }

  ProcessNode(scene->mRootNode, scene);
  if (success != nullptr)
  {
    *success = true;
  }
}

Model::Model(Model&& other):
  mDirectory(Util::Move(other.mDirectory)),
  mMeshes(Util::Move(other.mMeshes)),
  mLoadedTextures(Util::Move(other.mLoadedTextures))
{}

void Model::Draw(const Shader& shader) const
{
  for (const Mesh& mesh : mMeshes)
  {
    mesh.Draw(shader);
  }
}

void Model::ProcessNode(const aiNode* node, const aiScene* scene)
{
  // Add all of the meshes in this node to the Mesh array.
  for (unsigned int i = 0; i < node->mNumMeshes; ++i)
  {
    // With all of the models I have tested, mesh indicies are not repeated. I
    // have yet to see a model where the same mesh index is used multiple times.
    // That's why we aren't checking for already loaded meshes here.
    unsigned int meshIndex = node->mMeshes[i];
    const aiMesh* mesh = scene->mMeshes[meshIndex];
    mMeshes.Push(ProcessMesh(mesh, scene));
  }

  for (unsigned int i = 0; i < node->mNumChildren; ++i)
  {
    ProcessNode(node->mChildren[i], scene);
  }
}

Mesh Model::ProcessMesh(const aiMesh* mesh, const aiScene* scene)
{
  // Collect all of the vertex data from assimp.
  Ds::Vector<Mesh::Vertex> vertices;
  vertices.Reserve(mesh->mNumVertices);
  for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
  {
    Vec3 position;
    position[0] = mesh->mVertices[i].x;
    position[1] = mesh->mVertices[i].y;
    position[2] = mesh->mVertices[i].z;

    Vec3 normal;
    normal[0] = mesh->mNormals[i].x;
    normal[1] = mesh->mNormals[i].y;
    normal[2] = mesh->mNormals[i].z;

    // We only use the texture coordinates of the mesh if they exist. If the
    // mesh doesn't have any texture coordinates, we use [0, 0] as a default for
    // the texture coordinates.
    Vec2 texCoord;
    if (mesh->mTextureCoords[0])
    {
      texCoord[0] = mesh->mTextureCoords[0][i].x;
      texCoord[1] = mesh->mTextureCoords[0][i].y;
    } else
    {
      texCoord[0] = 0.0f;
      texCoord[1] = 0.0f;
    }
    Mesh::Vertex vertex = {position, normal, texCoord};
    vertices.Push(vertex);
  }

  // Collect all of the index data from assimp.
  Ds::Vector<unsigned int> indices;
  for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
  {
    const aiFace& face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; ++j)
    {
      indices.Push(face.mIndices[j]);
    }
  }

  // Collect all the textures and return the new Mesh.
  Ds::Vector<Texture> textures;
  if (mesh->mMaterialIndex >= 0)
  {
    const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    CollectMaterialTextures(&textures, material, TextureType::Diffuse);
    CollectMaterialTextures(&textures, material, TextureType::Specular);
  }
  return Mesh(Util::Move(vertices), Util::Move(indices), Util::Move(textures));
}

void Model::CollectMaterialTextures(
  Ds::Vector<Texture>* textures, const aiMaterial* material, TextureType type)
{
  aiTextureType aiType;
  switch (type)
  {
  case TextureType::Diffuse: aiType = aiTextureType_DIFFUSE; break;
  case TextureType::Specular: aiType = aiTextureType_SPECULAR; break;
  }

  // Collect all the textures of the given type and add them to the textures
  // vector.
  unsigned int count = material->GetTextureCount(aiType);
  for (unsigned int i = 0; i < count; ++i)
  {
    aiString aiTextureFile;
    material->GetTexture(aiType, i, &aiTextureFile);
    std::stringstream textureFile;
    textureFile << mDirectory << aiTextureFile.C_Str();

    // Before loading a texture, we check to see if mLoadedTextures already
    // contains the texture we need to avoid duplicate texture loads.
    // todo: An asset loading manager should be taking care of this rather than
    // baking a solution to this problem into the Model implementation.
    bool loaded = false;
    for (int j = 0; j < mLoadedTextures.Size(); ++j)
    {
      if (textureFile.str() == mLoadedTextures[j].mFile)
      {
        textures->Push(mLoadedTextures[j]);
        loaded = true;
        break;
      }
    }
    if (!loaded)
    {
      Texture newTexture(textureFile.str().c_str(), type);
      textures->Push(newTexture);
      mLoadedTextures.Push(newTexture);
    }
  }
}

} // namespace Gfx
