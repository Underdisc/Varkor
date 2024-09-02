#ifndef comp_Name_h
#define comp_Name_h

#include <string>

#include "vlk/Valkor.h"
#include "world/Object.h"

namespace Comp {

#pragma pack(push, 1)
struct Name
{
  void VInit(const World::Object& owner);
  void VSerialize(Vlk::Value& nameVal);
  void VDeserialize(const Vlk::Explorer& nameEx);
  void VEdit(const World::Object& owner);

  std::string mName;
};
#pragma pack(pop)

} // namespace Comp

#endif
