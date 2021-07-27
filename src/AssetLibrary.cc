#include <fstream>

#include "AssetLibrary.h"
#include "gfx/Model.h"
#include "gfx/Shader.h"
#include "util/Utility.h"
#include "vlk/Explorer.h"
#include "vlk/Pair.h"

namespace AssetLibrary {

template<typename T>
void SerializeAssets(Vlk::Pair& rootVlk)
{
  std::string resourceTypeName = Util::GetShortTypename<T>();
  Vlk::Pair& assetsVlk = rootVlk(resourceTypeName);
  for (const auto& idAssetPair : AssetBin<T>::smAssets)
  {
    if (idAssetPair.mValue.Required())
    {
      continue;
    }
    Vlk::Pair& assetVlk = assetsVlk(idAssetPair.mValue.mName);
    assetVlk("Id") = idAssetPair.Key();
    Vlk::Value& pathsVlk = assetVlk("Paths")[{T::smInitPathCount}];
    for (int i = 0; i < T::smInitPathCount; ++i)
    {
      pathsVlk[i] = idAssetPair.mValue.GetPath(i);
    }
  }
}

template<typename T>
void DeserializeAssets(const Vlk::Explorer& rootEx)
{
  std::string resourceTypename = Util::GetShortTypename<T>();
  Vlk::Explorer assetArrayEx = rootEx(resourceTypename);
  if (!assetArrayEx.Valid())
  {
    return;
  }
  for (int i = 0; i < assetArrayEx.Size(); ++i)
  {
    // Ensure we have a valid Id so the new asset can be added to the bin.
    Vlk::Explorer assetEx = assetArrayEx(i);
    AssetId id = assetEx("Id").As<int>(AssLib::nInvalidAssetId);
    if (id == AssLib::nInvalidAssetId)
    {
      std::stringstream error;
      error << assetEx.Path()
            << " asset omitted because it does not have a valid Id.";
      LogError(error.str().c_str());
      continue;
    }
    if (AssetBin<T>::smAssets.Contains(id))
    {
      std::stringstream error;
      error << assetEx.Path()
            << " asset omitted because its Id is already in use by another "
               "asset of the same type.";
      LogError(error.str().c_str());
      continue;
    }

    // Add the asset to the bin.
    Asset<T>& asset = AssetBin<T>::smAssets.Emplace(id, assetEx.Key(), false);
    if (id >= AssetBin<T>::smIdHandout)
    {
      AssetBin<T>::smIdHandout = id + 1;
    }
    Vlk::Explorer pathsEx = assetEx("Paths");
    for (int i = 0; i < T::smInitPathCount; ++i)
    {
      asset.SetPath(i, pathsEx[i].As<std::string>(""));
    }

    // todo: This will only be here temporarily. Assets should initialized in a
    // different thread upon request when they are needed.
    asset.Init();
  }
}

const char* nAssetFile = "assets.vlk";
void LoadAssets()
{
  Vlk::Pair rootVlk;
  Util::Result result = rootVlk.Read(nAssetFile);
  if (!result.Success())
  {
    LogError(result.mError.c_str());
    return;
  }
  Vlk::Explorer rootEx(rootVlk);
  DeserializeAssets<Gfx::Shader>(rootEx);
  DeserializeAssets<Gfx::Model>(rootEx);
}

void SaveAssets()
{
  Vlk::Pair rootVlk("Assets");
  SerializeAssets<Gfx::Shader>(rootVlk);
  SerializeAssets<Gfx::Model>(rootVlk);
  Util::Result result = rootVlk.Write(nAssetFile);
  LogErrorIf(!result.Success(), result.mError.c_str());
}

} // namespace AssetLibrary
