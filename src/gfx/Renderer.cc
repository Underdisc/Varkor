#include "AssetLibrary.h"
#include "Viewport.h"
#include "comp/Model.h"
#include "comp/Transform.h"
#include "gfx/Shader.h"
#include "math/Vector.h"
#include "world/Space.h"

namespace Gfx {
namespace Renderer {

Shader nDefaultShader;

void Init()
{
  Shader::InitResult result =
    nDefaultShader.Init("vres/default.vs", "vres/default.fs");
  LogAbortIf(!result.mSuccess, result.mError.c_str());

  Vec3 color = {0.0f, 1.0f, 0.0f};
  nDefaultShader.SetVec3("uColor", color.CData());
}

void RenderModels(const World::Space& space, const Mat4& view)
{
  // Visit all of the model components within the space.
  World::Table::Visitor<Comp::Model> visitor =
    space.CreateTableVisitor<Comp::Model>();
  while (!visitor.End())
  {
    // Find the shader that will be used to draw the model.
    const Comp::Model& modelComp = visitor.CurrentComponent();
    const AssetLibrary::Shader* shaderAsset =
      AssetLibrary::GetShader(modelComp.mShaderId);
    const Shader* drawShader;
    if (shaderAsset != nullptr && shaderAsset->mShader.Live())
    {
      drawShader = &shaderAsset->mShader;
    } else
    {
      drawShader = &nDefaultShader;
    }
    drawShader->Use();

    // If the object whose model component is being visited has a transfrom
    // component, we use the transformation matrix it provides.
    Comp::Transform* transform =
      space.GetComponent<Comp::Transform>(visitor.CurrentOwner());
    if (transform == nullptr)
    {
      Mat4 identity;
      Math::Identity(&identity);
      drawShader->SetMat4("uModel", identity.CData());
    } else
    {
      drawShader->SetMat4("uModel", transform->GetMatrix().CData());
    }
    drawShader->SetMat4("uView", view.CData());
    drawShader->SetMat4("uProj", Viewport::Perspective().CData());

    // We render the model referenced by the model component if the model has
    // been added to the asset library.
    const Gfx::Model* model = AssetLibrary::GetModel(modelComp.mAsset);
    if (model != nullptr)
    {
      model->Draw(*drawShader);
    }
    visitor.Next();
  }
}

void Render(const World::Space& space, const Mat4& view)
{
  RenderModels(space, view);
}

} // namespace Renderer
} // namespace Gfx
