#include "comp/Sprite.h"

namespace Comp {

void Sprite::VInit()
{
  mImageId = AssLib::nDefaultAssetId;
  mShaderId = AssLib::nDefaultAssetId;
}

void Sprite::VSerialize(Vlk::Value& spriteVal)
{
  spriteVal("ImageId") = mImageId;
  spriteVal("ShaderId") = mShaderId;
}

void Sprite::VDeserialize(const Vlk::Explorer& spriteEx)
{
  mImageId = spriteEx("ImageId").As<AssetId>(AssLib::nDefaultAssetId);
  mShaderId = spriteEx("ShaderId").As<AssetId>(AssLib::nDefaultAssetId);
}

} // namespace Comp
