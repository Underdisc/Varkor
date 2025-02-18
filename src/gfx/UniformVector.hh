#include "util/Memory.h"

namespace Gfx {

template<typename T>
UniformTypeId UniformType<T>::smTypeId = UniformTypeId::Invalid;

extern UniformTypeData nUniformTypeData[];

template<typename T>
void UniformType<T>::RegisterAttached(UniformTypeId typeId, const char* name) {
  if (smTypeId != UniformTypeId::Invalid) {
    UniformTypeData& typeData = nUniformTypeData[(int)smTypeId];
    std::string error = "Attached type \"";
    error += typeData.mName;
    error += "\" already registered.";
    LogAbort(error.c_str());
  }
  smTypeId = typeId;
  RegisterDetached(typeId, name);
}

template<typename T>
void UniformType<T>::RegisterDetached(UniformTypeId typeId, const char* name) {
  UniformTypeData& typeData = nUniformTypeData[(int)typeId];
  if (typeData.mName != nullptr) {
    UniformTypeData& typeData = nUniformTypeData[(int)typeId];
    std::string error = "Detached type \"";
    error += typeData.mName;
    error += "\" already registered.";
    LogAbort(error.c_str());
  }
  UniformTypeData newTypeData;
  newTypeData.mName = name;
  newTypeData.mSize = sizeof(T);
  newTypeData.mDefaultConstruct = Util::DefaultConstruct<T>;
  newTypeData.mMoveConstruct = Util::MoveConstruct<T>;
  newTypeData.mDestruct = Util::Destruct<T>;
  nUniformTypeData[(int)typeId] = newTypeData;
}

template<typename T>
UniformTypeId GetUniformTypeId() {
  bool invalid = UniformType<T>::smTypeId == UniformTypeId::Invalid;
  LogAbortIf(invalid, "Not registered as attached type.");
  return UniformType<T>::smTypeId;
}

template<typename T>
const UniformTypeData& GetUniformTypeData() {
  UniformTypeId typeId = GetUniformTypeId<T>();
  return GetUniformTypeData(typeId);
}

template<typename T>
T& UniformVector::Add(const std::string& name) {
  UniformTypeId typeId = GetUniformTypeId<T>();
  return *(T*)Add(typeId, name);
}

template<typename T>
void UniformVector::Add(const std::string& name, const T& value) {
  UniformTypeId typeId = GetUniformTypeId<T>();
  T& uniformValue = *(T*)Add(typeId, name);
  uniformValue = value;
}

template<typename T>
T& UniformVector::Add(UniformTypeId typeId, const std::string& name) {
  return *(T*)Add(typeId, name);
}

template<typename T>
void UniformVector::Add(
  UniformTypeId typeId, const std::string& name, const T& value) {
  T& uniformValue = *(T*)Add(typeId, name);
  uniformValue = value;
}

template<typename T>
T& UniformVector::Get(const std::string& name) {
  UniformTypeId typeId = GetUniformTypeId<T>();
  return *(T*)Get(typeId, name);
}

template<typename T>
void UniformVector::BindUniform(
  const Shader& shader, const UniformDescriptor& uniformDesc) const {
  char* uniformData = &mData[uniformDesc.mByteIndex];
  shader.SetUniform(uniformDesc.mName.c_str(), *(T*)uniformData);
}

} // namespace Gfx
