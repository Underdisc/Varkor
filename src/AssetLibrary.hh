namespace AssetLibrary {

template<typename T>
Asset<T>::Asset(const std::string& name): mName(name)
{}

template<typename T>
Util::Result Asset<T>::Init()
{
  return mResource.Init(mPaths);
}

template<typename T>
Util::Result Asset<T>::Init(const std::string paths[T::smInitPathCount])
{
  for (size_t i = 0; i < T::smInitPathCount; ++i)
  {
    mPaths[i] = paths[i];
  }
  return Init();
}

template<typename T>
void Asset<T>::SetPath(int index, const std::string& newPath)
{
  mPaths[index] = newPath;
  mResource.Purge();
}

template<typename T>
const std::string& Asset<T>::GetPath(int index) const
{
  return mPaths[index];
}

template<typename T>
AssetId Create(const std::string& name, bool includeId)
{
  AssetId id = AssetBin<T>::NextId();
  std::stringstream ss;
  ss << name;
  if (includeId)
  {
    ss << id;
  }
  AssetBin<T>::smAssets.Emplace(id, ss.str());
  return id;
}

template<typename T>
void Remove(AssetId id)
{
  AssetBin<T>::smAssets.Remove(id);
}

template<typename T, typename... Args>
AssetId Require(const std::string& name, Args&&... args)
{
  AssetId id = AssetBin<T>::NextRequiredId();
  Asset<T>& newAsset = AssetBin<T>::smAssets.Emplace(id, name);
  Util::Result result = newAsset.mResource.Init(args...);
  LogAbortIf(!result.Success(), result.mError.c_str());
  return id;
}

template<typename T>
T& Get(AssetId id, AssetId defaultId)
{
  return GetAsset<T>(id, defaultId).mResource;
}

template<typename T>
Asset<T>& GetAsset(AssetId id, AssetId defaultId)
{
  Asset<T>* asset = AssetBin<T>::smAssets.Find(id);
  if (asset == nullptr)
  {
    if (defaultId == nDefaultAssetId)
    {
      return AssetBin<T>::smDefault;
    }
    return AssetBin<T>::smAssets.Get(defaultId);
  }
  return *asset;
}

template<typename T>
T* TryGet(AssetId id)
{
  Asset<T>* asset = TryGetAsset<T>(id);
  if (asset == nullptr)
  {
    return nullptr;
  }
  return &asset->mResource;
}

template<typename T>
Asset<T>* TryGetAsset(AssetId id)
{
  if (id == 0)
  {
    return &AssetBin<T>::smDefault;
  }
  return AssetBin<T>::smAssets.Find(id);
}

template<typename T>
void AssetBin<T>::InitDefault(const std::string paths[T::smInitPathCount])
{
  Util::Result result = smDefault.Init(paths);
  LogAbortIf(!result.Success(), result.mError.c_str());
}

template<typename T>
AssetId AssetBin<T>::NextId()
{
  return smIdHandout++;
}

template<typename T>
AssetId AssetBin<T>::NextRequiredId()
{
  return smRequiredIdHandout--;
}

} // namespace AssetLibrary
