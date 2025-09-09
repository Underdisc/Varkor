#ifndef world_World_h
#define world_World_h

#include "Result.h"
#include "ds/List.h"
#include "rsl/ResourceId.h"
#include "world/Space.h"
#include "world/Types.h"

namespace World {

constexpr const char* nLayerExtension = ".lyr";

struct Layer {
  Layer();
  Layer(const std::string& name);

  std::string mName;
  std::string mFilename;
  MemberId mCameraId;
  ResId mPostMaterialId;
  ResId mTonemapMaterialId;
  ResId mIntenseExtractMaterialId;
  Space mSpace;
};

extern bool nPause;
extern Ds::List<Layer> nLayers;
typedef Ds::List<Layer>::Iter LayerIt;

extern void (*nCentralUpdate)();
extern void (*nLayerUpdate)(LayerIt layerIt);

void Init();
void Purge();
void Update();
LayerIt CreateTopLayer();
void DeleteLayer(LayerIt it);
VResult<LayerIt> LoadLayer(const char* filename);
Result SaveLayer(LayerIt it, const char* filename);

} // namespace World

#endif
