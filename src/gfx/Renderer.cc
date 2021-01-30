#include "AssetLibrary.h"
#include "Viewport.h"
#include "comp/Model.h"
#include "comp/Transform.h"
#include "core/Space.h"
#include "gfx/Shader.h"
#include "math/Vector.h"

namespace Gfx {
namespace Renderer {

Shader nDefaultShader;

void Init()
{
  nDefaultShader.Init("shader/light.vs", "shader/PureColor.fs");
  Vec3 color = {0.0f, 1.0f, 0.0f};
  nDefaultShader.SetVec3("uColor", color.CData());
}

void RenderModels(const Core::Space& space, const Shader& shader)
{
  // Visit all of the model components within the space.
  Core::Table::Visitor<Comp::Model> visitor =
    space.CreateTableVisitor<Comp::Model>();
  while (!visitor.End())
  {
    // If the object whose model component is being visited has a transfrom
    // component, we use transformation matrix it provides.
    Comp::Transform* transform =
      space.GetComponent<Comp::Transform>(visitor.CurrentOwner());
    if (transform == nullptr)
    {
      Mat4 identity;
      Math::Identity(&identity);
      shader.SetMat4("uModel", identity.CData());
    } else
    {
      shader.SetMat4("uModel", transform->GetMatrix().CData());
    }

    // We render the model referenced by the model component if the model has
    // been added to the asset library.
    const Comp::Model& modelComp = visitor.CurrentComponent();
    const Gfx::Model* model = AssetLibrary::GetModel(modelComp.mAsset);
    if (model != nullptr)
    {
      model->Draw(shader);
    }
    visitor.Next();
  }
}

void Render(const Core::Space& space, const Mat4& view)
{
  nDefaultShader.SetMat4("uView", view.CData());
  nDefaultShader.SetMat4("uProj", Viewport::Perspective().CData());
  RenderModels(space, nDefaultShader);
}

} // namespace Renderer
} // namespace Gfx
