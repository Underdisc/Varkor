#include "comp/Model.h"
#include "vlk/Valkor.h"

namespace Comp {

void Model::VInit()
{
  mModelId = AssLib::nDefaultAssetId;
  mShaderId = AssLib::nDefaultAssetId;
}

void Model::VSerialize(Vlk::Value& modelVal)
{
  modelVal("ShaderId") = mShaderId;
  modelVal("ModelId") = mModelId;
}

void Model::VDeserialize(const Vlk::Explorer& modelEx)
{
  mShaderId = modelEx("ShaderId").As<int>(AssLib::nDefaultAssetId);
  mModelId = modelEx("ModelId").As<int>(AssLib::nDefaultAssetId);
}

} // namespace Comp
