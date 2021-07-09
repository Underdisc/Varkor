namespace AssetLibrary {

template<typename T>
Asset<T>::Asset(const char* name, bool required):
  mName(name), mRequired(required)
{}

template<typename T>
Util::Result Asset<T>::Init()
{
  return mResource.Init(mPaths);
}

template<typename T>
bool Asset<T>::Required() const
{
  return mRequired;
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
  ss << name << id;
  AssetBin<T>::smAssets.Emplace(id, ss.str().c_str(), false);
  return id;
}

template<typename T>
void Remove(AssetId id)
{
  AssetBin<T>::smAssets.Remove(id);
}

template<typename T, typename... Args>
AssetId Require(Args&&... args)
{
  AssetId id = AssetBin<T>::NextId();
  Asset<T>& newAsset = AssetBin<T>::smAssets.Emplace(id, "vres", true);
  Util::Result result = newAsset.mResource.Init(args...);
  LogAbortIf(!result.Success(), result.mError.c_str());
  return id;
}

template<typename T>
T& Get(AssetId id)
{
  return GetAsset<T>(id).mResource;
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
Asset<T>& GetAsset(AssetId id)
{
  Asset<T>* asset = AssetBin<T>::smAssets.Find(id);
  if (asset == nullptr)
  {
    std::stringstream error;
    error << "No Asset<" << Util::GetShortTypename<T>() << "> with id: " << id;
    LogAbort(error.str().c_str());
  }
  return *asset;
}

template<typename T>
Asset<T>* TryGetAsset(AssetId id)
{
  return AssetBin<T>::smAssets.Find(id);
}

} // namespace AssetLibrary
