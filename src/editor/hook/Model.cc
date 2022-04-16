#include "editor/hook/Model.h"
#include "gfx/Model.h"
#include "gfx/Shader.h"

namespace Editor {

bool Hook<Comp::Model>::Edit(const World::Object& object)
{
  Comp::Model& model = object.GetComponent<Comp::Model>();
  SelectAssetWidget<Gfx::Model>(&model.mModelId);
  SelectAssetWidget<Gfx::Shader>(&model.mShaderId);
  return false;
}

} // namespace Editor
