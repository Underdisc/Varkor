namespace Rsl {

template<typename T>
ResId GetDefaultResId() {
  const ResTypeData& resTypeData = GetResTypeData<T>();
  return ResId(nDefaultAssetName, "Default");
}

template<typename T>
T& GetDefaultRes() {
  return GetRes<T>(GetDefaultResId<T>());
}

template<typename T>
T& GetRes(const ResId& resId) {
  Asset& asset = nAssets.Get(resId.GetAssetName());
  return asset.GetRes<T>(resId.GetResourceName());
}

template<typename T>
T* TryGetRes(const ResId& resId, const ResId& defaultResId) {
  std::string assetName = resId.GetAssetName();
  switch (GetAssetStatus(assetName)) {
  case Asset::Status::Dormant: QueueAsset(assetName);
  case Asset::Status::Queued:
  case Asset::Status::Initializing: return nullptr;
  case Asset::Status::Failed: return &GetDefaultRes<T>();
  case Asset::Status::Live: break;
  }
  Asset& asset = GetAsset(assetName);
  T* res = asset.TryGetRes<T>(resId.GetResourceName());
  if (res == nullptr) {
    if (defaultResId == "") {
      return &GetDefaultRes<T>();
    }
    return &GetRes<T>(defaultResId);
  }
  return res;
}

template<typename T>
bool HasRes(const ResId& resId) {
  Asset* asset = nAssets.TryGet(resId.GetAssetName());
  if (asset == nullptr) {
    return false;
  }
  return asset->HasRes<T>(resId.GetResourceName());
}

} // namespace Rsl
