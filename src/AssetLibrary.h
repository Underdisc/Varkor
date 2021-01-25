#ifndef AssetLibrary_h
#define AssetLibrary_h

#include <string>

#include "gfx/Model.h"

namespace AssetLibrary {

// Used for recording and checking whether an asset was successfully added to
// the asset library.
struct AddResult
{
  bool mSuccess;
  std::string mError;
};

AddResult AddModel(const std::string& path);
const Gfx::Model* GetModel(const std::string& path);

} // namespace AssetLibrary

#endif
