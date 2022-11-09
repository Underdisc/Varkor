namespace Rsl {

template<typename T, typename... Args>
T& Asset::InitRes(const std::string& name, Args&&... args)
{
  VResult<T*> result = TryInitRes<T>(name, std::forward<Args>(args)...);
  if (!result.Success()) {
    std::string error = "Resource \"" + mName + ":" + name +
      "\" failed initialization.\n" + result.mError;
    LogAbort(error.c_str());
  }
  return *result.mValue;
}

template<typename T, typename... Args>
VResult<T*> Asset::TryInitRes(const std::string& name, Args&&... args)
{
  T newRes;
  Result result = newRes.Init(std::forward<Args>(args)...);
  if (!result.Success()) {
    return result;
  }
  ResDesc newResDesc = AllocateRes(GetResTypeId<T>(), name);
  T* newResData = (T*)GetResDescData(newResDesc);
  const ResTypeData& typeData = GetResTypeData<T>();
  typeData.mMoveConstruct(&newRes, newResData);
  return VResult<T*>(std::move(result), newResData);
}

template<typename T>
T& Asset::GetRes(const std::string& name)
{
  T* res = TryGetRes<T>(name);
  if (res == nullptr) {
    std::string error =
      "Asset \"" + mName + "\" missing resource \"" + name + "\".";
    LogAbort(error.c_str());
  }
  return *res;
}

template<typename T>
T* Asset::TryGetRes(const std::string& name)
{
  ResTypeId requestedResTypeId = GetResTypeId<T>();
  for (const ResDesc& resDesc : mResDescs) {
    if (resDesc.mResTypeId == requestedResTypeId && resDesc.mName == name) {
      return (T*)GetResDescData(resDesc);
    }
  }
  return nullptr;
}

template<typename T>
bool Asset::HasRes(const std::string& name)
{
  ResTypeId resTypeId = GetResTypeId<T>();
  for (const ResDesc& resDesc : mResDescs) {
    if (resDesc.mResTypeId == resTypeId && resDesc.mName == name) {
      return true;
    }
  }
  return false;
}
} // namespace Rsl