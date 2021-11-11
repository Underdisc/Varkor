#include <fstream>

#include "AssetLibrary.h"
#include "gfx/Font.h"
#include "gfx/Image.h"
#include "gfx/Model.h"
#include "gfx/Shader.h"
#include "util/Utility.h"
#include "vlk/Explorer.h"
#include "vlk/Value.h"

namespace AssetLibrary {

template<typename T>
void SerializeAssets(Vlk::Value& rootVal)
{
  std::string resourceTypeName = Util::GetShortTypename<T>();
  Vlk::Value& assetsVal = rootVal(resourceTypeName);
  for (const auto& idAssetPair : AssetBin<T>::smAssets)
  {
    if (IsRequiredId(idAssetPair.Key()))
    {
      continue;
    }
    Vlk::Value& assetVal = assetsVal(idAssetPair.mValue.mName);
    assetVal("Id") = idAssetPair.Key();
    Vlk::Value& pathsVal = assetVal("Paths")[{T::smInitPathCount}];
    for (int i = 0; i < T::smInitPathCount; ++i)
    {
      pathsVal[i] = idAssetPair.mValue.GetPath(i);
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
    AssetId id = assetEx("Id").As<int>(AssLib::nDefaultAssetId);
    if (IsRequiredId(id))
    {
      std::stringstream error;
      error << assetEx.Path() << " asset omitted because it has a required Id.";
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
    Asset<T>& asset = AssetBin<T>::smAssets.Emplace(id, assetEx.Key());
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

void Init()
{
  std::string defaultFontPaths[Gfx::Font::smInitPathCount] = {
    "vres/font/NovaMono/font.ttf"};
  std::string defaultImagePaths[Gfx::Image::smInitPathCount] = {
    "vres/image/Default.png"};
  std::string defaultModelPaths[Gfx::Model::smInitPathCount] = {
    "vres/model/Default.obj"};
  std::string defaultShaderPaths[Gfx::Shader::smInitPathCount] = {
    "vres/shader/Default.vs", "vres/shader/Default.fs"};
  AssetBin<Gfx::Font>::InitDefault(defaultFontPaths);
  AssetBin<Gfx::Image>::InitDefault(defaultImagePaths);
  AssetBin<Gfx::Model>::InitDefault(defaultModelPaths);
  AssetBin<Gfx::Shader>::InitDefault(defaultShaderPaths);
  LoadAssets();
}

const char* nAssetFile = "assets.vlk";
void LoadAssets()
{
  Vlk::Value rootVal;
  Util::Result result = rootVal.Read(nAssetFile);
  if (!result.Success())
  {
    LogError(result.mError.c_str());
    return;
  }
  Vlk::Explorer rootEx(rootVal);
  DeserializeAssets<Gfx::Font>(rootEx);
  DeserializeAssets<Gfx::Image>(rootEx);
  DeserializeAssets<Gfx::Model>(rootEx);
  DeserializeAssets<Gfx::Shader>(rootEx);
}

void SaveAssets()
{
  Vlk::Value rootVal;
  SerializeAssets<Gfx::Font>(rootVal);
  SerializeAssets<Gfx::Image>(rootVal);
  SerializeAssets<Gfx::Model>(rootVal);
  SerializeAssets<Gfx::Shader>(rootVal);
  Util::Result result = rootVal.Write(nAssetFile);
  LogErrorIf(!result.Success(), result.mError.c_str());
}

bool IsRequiredId(AssetId id)
{
  return id < 1;
}

} // namespace AssetLibrary
