#ifndef rsl_ResourceType_h
#define rsl_ResourceType_h

#include "Error.h"
#include "util/Memory.h"

namespace Rsl {

enum class ResourceTypeId {
  Cubemap,
  Font,
  Image,
  Material,
  Mesh,
  Model,
  Shader,
  Count,
  Invalid,
};
typedef ResourceTypeId ResTypeId;

struct ResourceTypeData {
  size_t mSize;
  const char* mName;
  void (*mMoveConstruct)(void* from, void* to);
  void (*mDestruct)(void* at);
};
typedef ResourceTypeData ResTypeData;

template<typename T>
struct ResourceType {
  static ResTypeId smResTypeId;

private:
  static void Register(ResTypeId resTypeId, const char* name);
  friend void RegisterResourceTypes();
};
template<typename T>
ResTypeId ResourceType<T>::smResTypeId = ResTypeId::Invalid;

void RegisterResourceTypes();
template<typename T>
ResTypeId GetResTypeId();
ResTypeId GetResTypeId(const std::string& resTypeName);
template<typename T>
const ResTypeData& GetResTypeData();
const ResTypeData& GetResTypeData(ResTypeId resTypeId);
const ResTypeData& GetResTypeData(const std::string& resTypeName);

template<typename T>
ResTypeId GetResTypeId() {
  if (ResourceType<T>::smResTypeId == ResTypeId::Invalid) {
    LogAbort("Resource type not registered.");
  }
  return ResourceType<T>::smResTypeId;
}

template<typename T>
const ResourceTypeData& GetResTypeData() {
  return GetResTypeData(ResourceType<T>::smResTypeId);
}

} // namespace Rsl

typedef Rsl::ResourceTypeId ResTypeId;
typedef Rsl::ResourceTypeData ResTypeData;

#endif
