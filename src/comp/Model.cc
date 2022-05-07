#include "comp/Model.h"

namespace Comp {

void Model::VInit(const World::Object& owner)
{
  mModelId = AssLib::nDefaultAssetId;
  mShaderId = AssLib::nDefaultAssetId;
  mVisible = true;
}

void Model::VSerialize(Vlk::Value& modelVal)
{
  modelVal("ShaderId") = mShaderId;
  modelVal("ModelId") = mModelId;
  modelVal("Visible") = mVisible;
}

void Model::VDeserialize(const Vlk::Explorer& modelEx)
{
  mShaderId = modelEx("ShaderId").As<int>(AssLib::nDefaultAssetId);
  mModelId = modelEx("ModelId").As<int>(AssLib::nDefaultAssetId);
  mVisible = modelEx("Visible").As<bool>(true);
}

} // namespace Comp
