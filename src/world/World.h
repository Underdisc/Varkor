#ifndef world_World_h
#define world_World_h

#include "Result.h"
#include "ds/List.h"
#include "world/Space.h"
#include "world/Types.h"

namespace World {

struct Layer
{
  Layer();
  Layer(const std::string& name);

  std::string mName;
  MemberId mCameraId;
  Space mSpace;
};

extern bool nPause;
extern Ds::List<Layer> nLayers;
typedef Ds::List<Layer>::Iter LayerIt;

extern void (*nCentralUpdate)();
extern void (*nLayerUpdate)(LayerIt layerIt);

void Purge();
void Update();
LayerIt CreateTopLayer();
void DeleteLayer(LayerIt it);
ValueResult<LayerIt> LoadLayer(const char* filename);
Result SaveLayer(LayerIt it, const char* filename);

} // namespace World

#endif
