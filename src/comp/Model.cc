#include "comp/Model.h"
#include "vlk/Explorer.h"
#include "vlk/Value.h"

namespace Comp {

Model::Model():
  mShaderId(AssLib::nInvalidAssetId), mModelId(AssLib::nInvalidAssetId)
{}

void Model::VSerialize(Vlk::Value& modelVal)
{
  modelVal("ShaderId") = mShaderId;
  modelVal("ModelId") = mModelId;
}

void Model::VDeserialize(const Vlk::Explorer& modelEx)
{
  mShaderId = modelEx("ShaderId").As<int>(AssLib::nInvalidAssetId);
  mModelId = modelEx("ModelId").As<int>(AssLib::nInvalidAssetId);
}

} // namespace Comp
