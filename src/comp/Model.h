#ifndef comp_Model_h
#define comp_Model_h

#include <string>

namespace Comp {

#pragma pack(push, 1)
struct Model
{
  void EditorHook();
  std::string mAsset;
};
#pragma pack(pop)

} // namespace Comp

#endif
