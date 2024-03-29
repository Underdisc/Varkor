#include <sstream>
#include <utility>

#include "gfx/Renderer.h"
#include "vlk/Valkor.h"
#include "world/Registrar.h"
#include "world/World.h"

namespace World {

Layer::Layer(): mCameraId(nInvalidMemberId) {}

Layer::Layer(const std::string& name):
  mName(name),
  mCameraId(nInvalidMemberId),
  mPostMaterialId(Gfx::Renderer::nDefaultPostId)
{}

bool nPause = true;
Ds::List<Layer> nLayers;

// Function pointers for calling into project code.
void (*nCentralUpdate)() = nullptr;
void (*nLayerUpdate)(LayerIt layerIt) = nullptr;

void Init()
{
  Registrar::Init();
}

void Purge()
{
  nLayers.Clear();
}

void Update()
{
  if (nPause) {
    return;
  }
  if (nCentralUpdate != nullptr) {
    nCentralUpdate();
  }
  LayerIt it = nLayers.begin();
  LayerIt itE = nLayers.end();
  while (it != itE) {
    it->mSpace.Update();
    if (nLayerUpdate != nullptr) {
      nLayerUpdate(it);
    }
    ++it;
  }
}

LayerIt CreateTopLayer()
{
  std::stringstream defaultName;
  defaultName << "Layer" << nLayers.Size();
  return nLayers.Emplace(nLayers.end(), defaultName.str());
}

void DeleteLayer(LayerIt it)
{
  nLayers.Erase(it);
}

VResult<LayerIt> LoadLayer(const char* filename)
{
  Vlk::Value rootVal;
  Result result = rootVal.Read(filename);
  if (!result.Success()) {
    return VResult<LayerIt>(nLayers.end(), std::move(result));
  }
  Vlk::Explorer rootEx(rootVal);
  Vlk::Explorer metadataEx = rootEx("Metadata");
  nLayers.EmplaceBack();
  Layer& newLayer = *nLayers.Back();
  newLayer.mName = metadataEx("Name").As<std::string>("DefaultName");
  newLayer.mFilename = filename;
  newLayer.mCameraId = metadataEx("CameraId").As<MemberId>(nInvalidMemberId);
  Vlk::Explorer postMaterialEx = metadataEx("PostMaterialId");
  newLayer.mPostMaterialId =
    postMaterialEx.As<ResId>(Gfx::Renderer::nDefaultPostId);

  Vlk::Value& spaceVal = rootVal("Space");
  int progression =
    metadataEx("Progression").As<int>(Registrar::nInvalidProgression);
  Registrar::ProgressComponents(spaceVal, progression);

  Vlk::Explorer spaceEx(spaceVal);
  result = newLayer.mSpace.Deserialize(spaceEx);
  if (!result.Success()) {
    std::stringstream error;
    error << "Layer \"" << filename << "\" failed deserialization.\n"
          << result.mError;
    return VResult<LayerIt>(nLayers.end(), Result(error.str()));
  }
  return VResult<LayerIt>(nLayers.Back());
}

Result SaveLayer(LayerIt it, const char* filename)
{
  Layer& layer = *it;
  layer.mFilename = filename;
  Vlk::Value rootVal;
  Vlk::Value& metadataVal = rootVal("Metadata");
  metadataVal("Name") = layer.mName;
  metadataVal("CameraId") = layer.mCameraId;
  metadataVal("PostMaterialId") = layer.mPostMaterialId;
  metadataVal("Progression") = Registrar::nCurrentProgression;
  Vlk::Value& spaceVal = rootVal("Space");
  layer.mSpace.Serialize(spaceVal);
  return rootVal.Write(filename);
}

} // namespace World
