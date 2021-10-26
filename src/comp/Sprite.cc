#include "comp/Sprite.h"

namespace Comp {

Sprite::Sprite():
  mImageId(AssLib::nInvalidAssetId), mShaderId(AssLib::nInvalidAssetId)
{}

void Sprite::VSerialize(Vlk::Value& spriteVal)
{
  spriteVal("ImageId") = mImageId;
  spriteVal("ShaderId") = mShaderId;
}

void Sprite::VDeserialize(const Vlk::Explorer& spriteEx)
{
  mImageId = spriteEx("ImageId").As<AssetId>(AssLib::nInvalidAssetId);
  mShaderId = spriteEx("ShaderId").As<AssetId>(AssLib::nInvalidAssetId);
}

} // namespace Comp
