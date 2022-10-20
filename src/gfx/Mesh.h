#ifndef gfx_Mesh_h
#define gfx_Mesh_h

#include <assimp/mesh.h>
#include <glad/glad.h>

#include "Result.h"
#include "ds/Vector.h"
#include "vlk/Valkor.h"

namespace Gfx {

struct Mesh
{
  enum Attribute : unsigned int
  {
    Position = 1 << 0,
    Normal = 1 << 1,
    TexCoord = 1 << 2,
  };
  static unsigned int AttributesSize(unsigned int attributes);

  Mesh();
  Mesh(Mesh&& other);
  Mesh& operator=(Mesh&& other);
  ~Mesh();

  static void EditConfig(Vlk::Value* configValP);
  Result Init(const Vlk::Explorer& configEx);
  Result Init(const std::string& file, bool flipUvs = false);
  Result Init(const aiMesh& assimpMesh);
  Result Init(
    unsigned int attributes,
    const Ds::Vector<char>& vertexBuffer,
    const Ds::Vector<unsigned int>& elementBuffer);
  Result Init(
    void* vertexBuffer,
    size_t vertexBufferSize,
    unsigned int attributes,
    void* elementBuffer,
    size_t elementBufferSize,
    size_t elementCount);
  void Finalize();

  void Render() const;
  GLuint Vao() const;
  GLuint Ebo() const;
  size_t IndexCount() const;

private:
  unsigned int mAttributes;
  GLuint mVao, mVbo, mEbo;
  size_t mIndexCount;
};

} // namespace Gfx

#endif
