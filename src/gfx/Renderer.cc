#include <glad/glad.h>

#include "AssetLibrary.h"
#include "Viewport.h"
#include "comp/Model.h"
#include "comp/Transform.h"
#include "gfx/Shader.h"
#include "math/Vector.h"
#include "world/Object.h"
#include "world/Space.h"
#include "world/World.h"

namespace Gfx {
namespace Renderer {

Shader nDefaultShader;
Shader nMemberIdShader;

void Init()
{
  Shader::InitResult result =
    nDefaultShader.Init("vres/shader/Default.vs", "vres/shader/Default.fs");
  LogAbortIf(!result.mSuccess, result.mError.c_str());
  result =
    nMemberIdShader.Init("vres/shader/Default.vs", "vres/shader/MemberId.fs");
  LogAbortIf(!result.mSuccess, result.mError.c_str());

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT, GL_FILL);
}

void Clear()
{
  glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

Mat4 GetTransformation(const World::Space& space, World::MemberId memberId)
{
  Comp::Transform* transform = space.GetComponent<Comp::Transform>(memberId);
  if (transform == nullptr)
  {
    Mat4 identity;
    Math::Identity(&identity);
    return identity;
  }
  return transform->GetWorldMatrix(space, memberId);
}

void RenderModel(const Shader& shader, const Comp::Model& modelComp)
{
  const Gfx::Model* model = AssetLibrary::GetModel(modelComp.mAsset);
  if (model != nullptr)
  {
    model->Draw(shader);
  }
}

void RenderMemberIds(const World::Space& space, const Mat4& view)
{
  nMemberIdShader.Use();
  nMemberIdShader.SetMat4("uView", view.CData());
  nMemberIdShader.SetMat4("uProj", Viewport::Perspective().CData());

  // Render every model in the space and use the model owner's MemberId as the
  // color value.
  World::Table::Visitor<Comp::Model> visitor =
    space.CreateTableVisitor<Comp::Model>();
  while (!visitor.End())
  {
    Mat4 model = GetTransformation(space, visitor.CurrentOwner());
    nMemberIdShader.SetMat4("uModel", model.CData());
    nMemberIdShader.SetInt("uMemberId", visitor.CurrentOwner());
    const Comp::Model& modelComp = visitor.CurrentComponent();
    RenderModel(nMemberIdShader, modelComp);
    visitor.Next();
  }
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

    // Draw the model.
    drawShader->Use();
    Mat4 model = GetTransformation(space, visitor.CurrentOwner());
    drawShader->SetMat4("uModel", model.CData());
    drawShader->SetMat4("uView", view.CData());
    drawShader->SetMat4("uProj", Viewport::Perspective().CData());
    RenderModel(*drawShader, modelComp);
    visitor.Next();
  }
}

void RenderSpace(const World::Space& space, const Mat4& view)
{
  RenderModels(space, view);
}

void RenderSpace(const World::Space& space)
{
  // Find the view matrix using the space's camera and render the space.
  Comp::Transform* cameraTransform = nullptr;
  if (space.mCameraId != World::nInvalidMemberId)
  {
    cameraTransform = space.GetComponent<Comp::Transform>(space.mCameraId);
  }
  Mat4 view;
  if (cameraTransform == nullptr)
  {
    Math::Identity(&view);
  } else
  {
    view = cameraTransform->GetWorldMatrix(space, space.mCameraId);
    view = Math::Inverse(view);
  }
  RenderModels(space, view);
}

void RenderWorld()
{
  World::SpaceVisitor visitor;
  while (!visitor.End())
  {
    RenderSpace(visitor.CurrentSpace());
    visitor.Next();
  }
}

} // namespace Renderer
} // namespace Gfx
