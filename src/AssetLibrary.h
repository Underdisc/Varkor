#ifndef AssetLibrary_h
#define AssetLibrary_h

#include <string>

#include "ds/Map.h"
#include "gfx/Model.h"
#include "gfx/Shader.h"

namespace AssetLibrary {

struct Shader
{
  std::string mName;
  std::string mVertexFile;
  std::string mFragmentFile;
  Gfx::Shader mShader;
};
typedef int ShaderId;
constexpr ShaderId nInvalidShaderId = -1;
extern Ds::Map<ShaderId, Shader> nShaders;

void CreateEmptyShader();
const Shader* GetShader(ShaderId shaderId);

// Used for recording and checking whether an asset was successfully added to
// the asset library.
struct AddResult
{
  bool mSuccess;
  std::string mError;
};
AddResult AddModel(const std::string& path);
void AddRequiredModel(const std::string& path);
const Gfx::Model* GetModel(const std::string& path);

} // namespace AssetLibrary

#endif
