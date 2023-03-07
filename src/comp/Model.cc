#include "comp/Model.h"
#include "comp/Transform.h"
#include "editor/Utility.h"
#include "gfx/Model.h"
#include "rsl/Library.h"

namespace Comp {

void Model::VInit(const World::Object& owner)
{
  mModelId = Rsl::GetDefaultResId<Gfx::Model>();
  mVisible = true;
}

void Model::VSerialize(Vlk::Value& val)
{
  val("ModelId") = mModelId;
  val("Visible") = mVisible;
}

void Model::VDeserialize(const Vlk::Explorer& modelEx)
{
  mModelId = modelEx("ModelId").As<ResId>(Rsl::GetDefaultResId<Gfx::Model>());
  mVisible = modelEx("Visible").As<bool>(true);
}

void Model::VRenderable(const World::Object& owner)
{
  if (!mVisible) {
    return;
  }
  const Gfx::Model* model = Rsl::TryGetRes<Gfx::Model>(mModelId);
  if (model == nullptr) {
    return;
  }

  Mat4 ownerTransform = owner.Get<Comp::Transform>().GetWorldMatrix(owner);
  for (size_t i = 0; i < model->RenderableCount(); ++i) {
    Gfx::Renderable::Floater floater = model->GetFloater(i);
    floater.mOwner = owner.mMemberId;
    floater.mTransform = ownerTransform * floater.mTransform;
    Gfx::Collection::Add(std::move(floater));
  }
}

void Model::VEdit(const World::Object& owner)
{
  Editor::DropResourceIdWidget(Rsl::ResTypeId::Model, &mModelId);
  ImGui::Checkbox("Visible", &mVisible);
}

} // namespace Comp