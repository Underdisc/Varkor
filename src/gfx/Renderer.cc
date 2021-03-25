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
    nDefaultShader.Init("vres/default.vs", "vres/default.fs");
  LogAbortIf(!result.mSuccess, result.mError.c_str());
  result = nMemberIdShader.Init("vres/default.vs", "vres/MemberId.fs");
  LogAbortIf(!result.mSuccess, result.mError.c_str());

  Vec3 color = {0.0f, 1.0f, 0.0f};
  nDefaultShader.SetVec3("uColor", color.CData());

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

Mat4 GetTransformation(
  const World::Space& space, World::SpaceId spaceId, World::MemberId memberId)
{
  Comp::Transform* transform = space.GetComponent<Comp::Transform>(memberId);
  if (transform == nullptr)
  {
    Mat4 identity;
    Math::Identity(&identity);
    return identity;
  }
  World::Object object(spaceId, memberId);
  return transform->GetWorldMatrix(object);
}

void RenderModel(const Shader& shader, const Comp::Model& modelComp)
{
  const Gfx::Model* model = AssetLibrary::GetModel(modelComp.mAsset);
  if (model != nullptr)
  {
    model->Draw(shader);
  }
}

void RenderMemberIds(World::SpaceId spaceId, const Mat4& view)
{
  nMemberIdShader.Use();
  nMemberIdShader.SetMat4("uView", view.CData());
  nMemberIdShader.SetMat4("uProj", Viewport::Perspective().CData());

  // Render every model in the space and use the model owner's MemberId as the
  // color value.
  const World::Space& space = World::GetSpace(spaceId);
  World::Table::Visitor<Comp::Model> visitor =
    space.CreateTableVisitor<Comp::Model>();
  while (!visitor.End())
  {
    Mat4 model = GetTransformation(space, spaceId, visitor.CurrentOwner());
    nMemberIdShader.SetMat4("uModel", model.CData());
    nMemberIdShader.SetInt("uMemberId", visitor.CurrentOwner());
    const Comp::Model& modelComp = visitor.CurrentComponent();
    RenderModel(nMemberIdShader, modelComp);
    visitor.Next();
  }
}

void RenderModels(World::SpaceId spaceId, const Mat4& view)
{
  // Visit all of the model components within the space.
  const World::Space& space = World::GetSpace(spaceId);
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
    Mat4 model = GetTransformation(space, spaceId, visitor.CurrentOwner());
    drawShader->SetMat4("uModel", model.CData());
    drawShader->SetMat4("uView", view.CData());
    drawShader->SetMat4("uProj", Viewport::Perspective().CData());
    RenderModel(*drawShader, modelComp);
    visitor.Next();
  }
}

void RenderSpace(World::SpaceId spaceId, const Mat4& view)
{
  RenderModels(spaceId, view);
}

void RenderSpace(World::SpaceId spaceId)
{
  // Find the view matrix using the space's camera and render the space.
  const World::Space& space = World::GetSpace(spaceId);
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
    World::Object cameraOwner(spaceId, space.mCameraId);
    view = Math::Inverse(cameraTransform->GetWorldMatrix(cameraOwner));
  }
  RenderModels(spaceId, view);
}

void RenderWorld()
{
  World::SpaceVisitor visitor;
  while (!visitor.End())
  {
    RenderSpace(visitor.CurrentSpaceId());
    visitor.Next();
  }
}

} // namespace Renderer
} // namespace Gfx
