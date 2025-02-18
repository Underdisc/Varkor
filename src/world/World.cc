#include <sstream>
#include <utility>

#include "Log.h"
#include "gfx/Renderer.h"
#include "vlk/Valkor.h"
#include "world/Registrar.h"
#include "world/World.h"

namespace World {

Layer::Layer(): mCameraId(nInvalidMemberId) {}

Layer::Layer(const std::string& name):
  mName(name),
  mCameraId(nInvalidMemberId),
  mPostMaterialId(Gfx::Renderer::nDefaultPostId) {}

bool nPause = true;
Ds::List<Layer> nLayers;

// Function pointers for calling into project code.
void (*nCentralUpdate)() = nullptr;
void (*nLayerUpdate)(LayerIt layerIt) = nullptr;

void Init() {
  Registrar::Init();
}

void Purge() {
  nLayers.Clear();
}

void Update() {
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

LayerIt CreateTopLayer() {
  std::stringstream defaultName;
  defaultName << "Layer" << nLayers.Size();
  return nLayers.Emplace(nLayers.end(), defaultName.str());
}

void DeleteLayer(LayerIt it) {
  nLayers.Erase(it);
}

VResult<LayerIt> LoadLayer(const char* filename) {
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

  // Progress the layer forward.
  int layerProgression =
    metadataEx("LayerProgression").As<int>(Registrar::nInvalidProgression);
  if (layerProgression < Registrar::nCurrentLayerProgression) {
    Registrar::ProgressLayer(rootVal, layerProgression);
    std::stringstream logString;
    logString << "Progressed \"" << filename << "\" from layer progression "
              << std::to_string(layerProgression) << " to "
              << std::to_string(Registrar::nCurrentLayerProgression);
    Log::String(logString.str());
  }

  // Progress all components forward.
  Vlk::Value& spaceVal = rootVal("Space");
  int progression =
    metadataEx("ComponentProgression").As<int>(Registrar::nInvalidProgression);
  if (progression < Registrar::nCurrentComponentProgression) {
    Registrar::ProgressComponents(spaceVal, progression);
    std::string logString = "Progressed \"";
    logString += filename;
    logString += "\" from component progression " +
      std::to_string(progression) + " to " +
      std::to_string(Registrar::nCurrentComponentProgression);
    Log::String(logString);
  }

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

Result SaveLayer(LayerIt it, const char* filename) {
  Layer& layer = *it;
  layer.mFilename = filename;
  Vlk::Value rootVal;
  Vlk::Value& metadataVal = rootVal("Metadata");
  metadataVal("Name") = layer.mName;
  metadataVal("CameraId") = layer.mCameraId;
  metadataVal("PostMaterialId") = layer.mPostMaterialId;
  metadataVal("ComponentProgression") = Registrar::nCurrentComponentProgression;
  metadataVal("LayerProgression") = Registrar::nCurrentLayerProgression;
  Vlk::Value& spaceVal = rootVal("Space");
  layer.mSpace.Serialize(spaceVal);
  return rootVal.Write(filename);
}

} // namespace World
