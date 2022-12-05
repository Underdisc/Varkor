namespace Rsl {

template<typename T>
ResId GetDefaultResId()
{
  const ResTypeData& resTypeData = GetResTypeData<T>();
  return ResId(nDefaultAssetName, resTypeData.mName);
}

template<typename T>
T& GetDefaultRes()
{
  return GetRes<T>(GetDefaultResId<T>());
}

template<typename T>
T& GetRes(const ResId& resId)
{
  Asset& asset = nAssets.Get(resId.GetAssetName());
  return asset.GetRes<T>(resId.GetResourceName());
}

template<typename T>
T* TryGetRes(const ResId& resId)
{
  std::string assetName = resId.GetAssetName();
  Asset* asset = TryGetAsset(assetName);
  if (asset == nullptr) {
    QueueAsset(assetName);
    return nullptr;
  }
  if (asset->GetStatus() == Asset::Status::Initializing) {
    return nullptr;
  }
  T* res = asset->TryGetRes<T>(resId.GetResourceName());
  if (res != nullptr) {
    return res;
  }
  switch (asset->GetStatus()) {
  case Asset::Status::Live:
  case Asset::Status::Failed: return &GetDefaultRes<T>();
  case Asset::Status::Dormant: asset->QueueInit();
  }
  return nullptr;
}

template<typename T>
bool HasRes(const ResId& resId)
{
  Asset* asset = nAssets.TryGet(resId.GetAssetName());
  if (asset == nullptr) {
    return false;
  }
  return asset->HasRes<T>(resId.GetResourceName());
}

} // namespace Rsl