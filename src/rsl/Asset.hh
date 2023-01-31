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
  Result initResult = newRes.Init(std::forward<Args>(args)...);
  if (!initResult.Success()) {
    return initResult;
  }
  VResult<ResDesc> allocResult = AllocateRes(Rsl::GetResTypeId<T>(), name);
  if (!allocResult.Success()) {
    return std::move(allocResult);
  }

  T* newResData = (T*)GetResDescData(allocResult.mValue);
  const ResTypeData& typeData = Rsl::GetResTypeData<T>();
  typeData.mMoveConstruct(&newRes, newResData);
  return newResData;
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
  ResTypeId requestedResTypeId = Rsl::GetResTypeId<T>();
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
  ResTypeId resTypeId = Rsl::GetResTypeId<T>();
  for (const ResDesc& resDesc : mResDescs) {
    if (resDesc.mResTypeId == resTypeId && resDesc.mName == name) {
      return true;
    }
  }
  return false;
}

} // namespace Rsl