#include "editor/hook/Sprite.h"
#include "gfx/Image.h"
#include "gfx/Shader.h"

namespace Editor {

bool Hook<Comp::Sprite>::Edit(const World::Object& object)
{
  Comp::Sprite* sprite = object.GetComponent<Comp::Sprite>();
  SelectAssetWidget<Gfx::Image>(&sprite->mImageId);
  SelectAssetWidget<Gfx::Shader>(&sprite->mShaderId);
  return false;
}

} // namespace Editor
