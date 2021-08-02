#include "comp/Model.h"
#include "vlk/Explorer.h"
#include "vlk/Pair.h"

namespace Comp {

Model::Model():
  mShaderId(AssLib::nInvalidAssetId), mModelId(AssLib::nInvalidAssetId)
{}

void Model::VSerialize(Vlk::Pair& modelVlk)
{
  modelVlk("ShaderId") = mShaderId;
  modelVlk("ModelId") = mModelId;
}

void Model::VDeserialize(const Vlk::Explorer& modelEx)
{
  mShaderId = modelEx("ShaderId").As<int>(AssLib::nInvalidAssetId);
  mModelId = modelEx("ModelId").As<int>(AssLib::nInvalidAssetId);
}

} // namespace Comp
