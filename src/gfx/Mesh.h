#ifndef gfx_Mesh_h
#define gfx_Mesh_h

#include <assimp/mesh.h>
#include <glad/glad.h>

#include "Result.h"
#include "ds/Vector.h"
#include "math/Vector.h"
#include "vlk/Valkor.h"

namespace Gfx {

struct Mesh
{
  enum Attribute : unsigned int
  {
    Position = 1 << 0,
    Tangent = 1 << 1,
    Bitagent = 1 << 2,
    Normal = 1 << 3,
    TexCoord = 1 << 4,
  };
  static unsigned int AttributesSize(unsigned int attributes);

  Mesh();
  Mesh(Mesh&& other);
  Mesh& operator=(Mesh&& other);
  ~Mesh();

  static void EditConfig(Vlk::Value* configValP);
  Result Init();
  Result Init(const Vlk::Explorer& configEx);
  Result Init(const std::string& file, bool flipUvs, float scale);
  Result Init(const aiMesh& assimpMesh, float scale);
  Result Init(
    unsigned int attributes,
    const Ds::Vector<Vec3>& vertices,
    const Ds::Vector<unsigned int>& indices);
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
  void UpdateVbo(size_t byteOffset, size_t byteCount, const void* data) const;
  void Purge();

  void Render() const;
  GLuint Vao() const;
  GLuint Ebo() const;
  bool Initialized() const;

  size_t mIndexCount;

private:
  unsigned int mAttributes;
  GLuint mVao, mVbo, mEbo;
};

} // namespace Gfx

#endif
