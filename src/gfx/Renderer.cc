#include <glad/glad.h>
#include <sstream>
#include <utility>

#include "Input.h"
#include "Temporal.h"
#include "Viewport.h"
#include "comp/AlphaColor.h"
#include "comp/Camera.h"
#include "comp/DirectionalLight.h"
#include "comp/PointLight.h"
#include "comp/ShadowMap.h"
#include "comp/SpotLight.h"
#include "comp/Transform.h"
#include "debug/Draw.h"
#include "ds/Vector.h"
#include "editor/Editor.h"
#include "editor/LayerInterface.h"
#include "gfx/Framebuffer.h"
#include "gfx/Material.h"
#include "gfx/Mesh.h"
#include "gfx/Renderable.h"
#include "gfx/Renderer.h"
#include "gfx/Shader.h"
#include "math/Vector.h"
#include "rsl/Asset.h"
#include "rsl/Library.h"
#include "world/Object.h"
#include "world/Space.h"
#include "world/World.h"

namespace Gfx {
namespace Renderer {

GLuint nUniversalUniformBufferVbo;
GLuint nLightsUniformBufferVbo;
GLuint nShadowUniformBufferVbo;

size_t nCurrentSpaceFramebufferIndex = 0;
Ds::Vector<Framebuffer> nSpaceFramebuffers;

const char* nRendererAssetName = "vres/renderer";
const ResId nFullscreenMeshId(nRendererAssetName, "FullscreenMesh");
const ResId nSpriteMeshId(nRendererAssetName, "SpriteMesh");
const ResId nSkyboxMeshId(nRendererAssetName, "SkyboxMesh");
const ResId nMemberIdShaderId(nRendererAssetName, "MemberIdShader");
const ResId nDepthShaderId(nRendererAssetName, "DepthShader");
const ResId nDefaultPostShaderId(nRendererAssetName, "DefaultPostShader");
const ResId nDefaultPostMaterialId(nRendererAssetName, "DefaultPostMaterial");

void (*nCustomRender)() = nullptr;

void Init()
{
  // Initialize all of the resources used by the renderer.
  Rsl::Asset& asset = Rsl::GetAsset(nRendererAssetName);
  // clang-format off
  float fullscreenVertices[] =
    {-1.0f,  1.0f, 0.0f,
     -1.0f, -1.0f, 0.0f,
      1.0f,  1.0f, 0.0f,
      1.0f, -1.0f, 0.0f };
  float spriteVertices[] =
    {-0.5f,  0.5f, 0.0f,
     -0.5f, -0.5f, 0.0f,
      0.5f,  0.5f, 0.0f,
      0.5f, -0.5f, 0.0f};
  // clang-format on
  unsigned int quadElements[] = {0, 1, 2, 1, 3, 2};
  asset.InitRes<Gfx::Mesh>(
    nFullscreenMeshId.GetResourceName(),
    (void*)fullscreenVertices,
    sizeof(fullscreenVertices),
    Gfx::Mesh::Attribute::Position,
    (void*)quadElements,
    sizeof(quadElements),
    sizeof(quadElements) / sizeof(unsigned int));
  asset.InitRes<Gfx::Mesh>(
    nSpriteMeshId.GetResourceName(),
    (void*)spriteVertices,
    sizeof(spriteVertices),
    Gfx::Mesh::Attribute::Position,
    (void*)quadElements,
    sizeof(quadElements),
    sizeof(quadElements) / sizeof(unsigned int));
  // clang-format off
  float skyboxVertices[] = {
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
  };
  unsigned int skyboxElements[] = {
    0, 1, 2,
    0, 2, 3,
    0, 7, 4,
    0, 3, 7,
    0, 4, 5,
    0, 5, 1,
    6, 4, 7,
    6, 5, 4,
    6, 3, 2,
    6, 7, 3,
    6, 1, 5,
    6, 2, 1,
  };
  // clang-format on
  unsigned int skyboxElementsSize =
    sizeof(skyboxElements) / sizeof(unsigned int);
  asset.InitRes<Gfx::Mesh>(
    nSkyboxMeshId.GetResourceName(),
    (void*)skyboxVertices,
    sizeof(skyboxVertices),
    Gfx::Mesh::Attribute::Position,
    (void*)skyboxElements,
    sizeof(skyboxElements),
    sizeof(skyboxElements) / sizeof(unsigned int));
  asset.InitFinalize();

  // Create the uniform buffers.
  auto CreateUniformBuffer = [](GLuint* vbo, GLsizeiptr size, GLuint binding)
  {
    glGenBuffers(1, vbo);
    glBindBuffer(GL_UNIFORM_BUFFER, *vbo);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, *vbo);
  };
  CreateUniformBuffer(&nUniversalUniformBufferVbo, 144, 0);
  CreateUniformBuffer(&nLightsUniformBufferVbo, 17680, 1);
  CreateUniformBuffer(&nShadowUniformBufferVbo, 80, 2);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Purge()
{
  nSpaceFramebuffers.Clear();
}

void Clear()
{
  // Remove any space framebuffers that were not used during the last render and
  // clear those that were used.
  size_t popCount = nSpaceFramebuffers.Size() - nCurrentSpaceFramebufferIndex;
  for (size_t i = 0; i < popCount; ++i) {
    nSpaceFramebuffers.Pop();
  }
  for (size_t i = 0; i < nSpaceFramebuffers.Size(); ++i) {
    glBindFramebuffer(GL_FRAMEBUFFER, nSpaceFramebuffers[i].Fbo());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  nCurrentSpaceFramebufferIndex = 0;

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Render()
{
  Clear();
  if (nCustomRender != nullptr) {
    nCustomRender();
  }
  if (Editor::nEditorMode) {
    // Render the selected space and the editor space.
    Editor::LayerInterface* layerInterface =
      Editor::nCoreInterface.FindInterface<Editor::LayerInterface>();
    if (layerInterface == nullptr) {
      return;
    }
    const World::Layer& worldLayer = *layerInterface->mLayerIt;
    const World::Space& space = worldLayer.mSpace;
    const Mat4& view = Editor::nCamera.View();
    const Mat4& proj = Editor::nCamera.Proj();
    const Vec3& viewPos = Editor::nCamera.Position();
    RenderSpace(space, view, proj, viewPos, worldLayer.mPostMaterialId);
    RenderSpace(Editor::nSpace, view, proj, viewPos, nDefaultPostMaterialId);
    Debug::Draw::Render(Editor::nCamera.View(), Editor::nCamera.Proj());
  }
  else {
    Result result = RenderWorld();
    if (!result.Success()) {
      LogError(result.mError.c_str());
      Editor::nEditorMode = true;
    }
  }
}

Result RenderWorld()
{
  for (const World::Layer& layer : World::nLayers) {
    // Make sure the layer has a camera.
    if (layer.mCameraId == World::nInvalidMemberId) {
      std::stringstream error;
      error << "\"" << layer.mName << "\" layer has no assigned camera.";
      return Result(error.str());
    }
    // Make sure the camera has a camera component.
    const World::Space& space = layer.mSpace;
    const auto* cameraComp = space.TryGet<Comp::Camera>(layer.mCameraId);
    if (cameraComp == nullptr) {
      std::stringstream error;
      error << "\"" << layer.mName << "\" layer camera has no camera component";
      return Result(error.str());
    }

    // Render the space using the layer camera.
    auto& transformComp = space.GetComponent<Comp::Transform>(layer.mCameraId);
    World::Object object(const_cast<World::Space*>(&space), layer.mCameraId);
    Mat4 view = transformComp.GetInverseWorldMatrix(object);
    Mat4 proj = cameraComp->Proj();
    Vec3 viewPos = transformComp.GetWorldTranslation(object);
    RenderSpace(space, view, proj, viewPos, layer.mPostMaterialId);
    Debug::Draw::Render(view, proj);
  }
  return Result();
}

void InitializeUniversalUniformBuffer(
  const Mat4& view, const Mat4& proj, const Vec3& viewPos)
{
  Mat4 viewTranspose = Math::Transpose(view);
  Mat4 projTranspose = Math::Transpose(proj);
  float totalTime = Temporal::TotalTime();
  glBindBuffer(GL_UNIFORM_BUFFER, nUniversalUniformBufferVbo);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Mat4), viewTranspose.CData());
  glBufferSubData(GL_UNIFORM_BUFFER, 64, sizeof(Mat4), projTranspose.CData());
  glBufferSubData(GL_UNIFORM_BUFFER, 128, sizeof(Vec3), viewPos.CData());
  glBufferSubData(GL_UNIFORM_BUFFER, 140, sizeof(float), &totalTime);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void InitializeLightsUniformBuffer(const World::Space& space)
{
  GLenum buffer = GL_UNIFORM_BUFFER;
  glBindBuffer(buffer, nLightsUniformBufferVbo);

  const unsigned int maxDirectionalLights = 1;
  unsigned int directionalLightCount = 0;
  GLintptr offset = 16;
  Ds::Vector<World::MemberId> slice = space.Slice<Comp::DirectionalLight>();
  for (int i = 0; i < slice.Size() && i < maxDirectionalLights; ++i) {
    auto& light = space.Get<Comp::DirectionalLight>(slice[i]);
    Vec3 trueAmbient = light.mAmbient.TrueColor();
    Vec3 trueDiffuse = light.mDiffuse.TrueColor();
    Vec3 trueSpecular = light.mSpecular.TrueColor();
    glBufferSubData(buffer, offset, sizeof(Vec3), light.mDirection.mD);
    glBufferSubData(buffer, offset + 16, sizeof(Vec3), trueAmbient.mD);
    glBufferSubData(buffer, offset + 32, sizeof(Vec3), trueDiffuse.mD);
    glBufferSubData(buffer, offset + 48, sizeof(Vec3), trueSpecular.mD);
    offset += 64;
    ++directionalLightCount;
  }

  const unsigned int maxPointLights = 100;
  unsigned int pointLightCount = 0;
  offset = 16 + maxDirectionalLights * 64;
  slice = std::move(space.Slice<Comp::PointLight>());
  for (int i = 0; i < slice.Size() && i < maxPointLights; ++i) {
    auto& light = space.Get<Comp::PointLight>(slice[i]);
    Vec3 trueAmbient = light.mAmbient.TrueColor();
    Vec3 trueDiffuse = light.mDiffuse.TrueColor();
    Vec3 trueSpecular = light.mSpecular.TrueColor();
    glBufferSubData(buffer, offset, sizeof(Vec3), light.mPosition.mD);
    glBufferSubData(buffer, offset + 16, sizeof(Vec3), trueAmbient.mD);
    glBufferSubData(buffer, offset + 32, sizeof(Vec3), trueDiffuse.mD);
    glBufferSubData(buffer, offset + 48, sizeof(Vec3), trueSpecular.mD);
    glBufferSubData(buffer, offset + 60, sizeof(float), &light.mConstant);
    glBufferSubData(buffer, offset + 64, sizeof(float), &light.mLinear);
    glBufferSubData(buffer, offset + 68, sizeof(float), &light.mQuadratic);
    offset += 80;
    ++pointLightCount;
  }

  const unsigned int maxSpotLights = 100;
  unsigned int spotLightCount = 0;
  offset = 16 + maxDirectionalLights * 64 + maxPointLights * 80;
  slice = std::move(space.Slice<Comp::SpotLight>());
  for (int i = 0; i < slice.Size() && i < maxSpotLights; ++i) {
    auto& light = space.Get<Comp::SpotLight>(slice[i]);
    Vec3 trueAmbient = light.mAmbient.TrueColor();
    Vec3 trueDiffuse = light.mDiffuse.TrueColor();
    Vec3 trueSpecular = light.mSpecular.TrueColor();
    glBufferSubData(buffer, offset, sizeof(Vec3), light.mPosition.mD);
    glBufferSubData(buffer, offset + 16, sizeof(Vec3), light.mDirection.mD);
    glBufferSubData(buffer, offset + 32, sizeof(Vec3), trueAmbient.mD);
    glBufferSubData(buffer, offset + 48, sizeof(Vec3), trueDiffuse.mD);
    glBufferSubData(buffer, offset + 64, sizeof(Vec3), trueSpecular.mD);
    glBufferSubData(buffer, offset + 76, sizeof(float), &light.mConstant);
    glBufferSubData(buffer, offset + 80, sizeof(float), &light.mLinear);
    glBufferSubData(buffer, offset + 84, sizeof(float), &light.mQuadratic);
    glBufferSubData(buffer, offset + 88, sizeof(float), &light.mInnerCutoff);
    glBufferSubData(buffer, offset + 92, sizeof(float), &light.mOuterCutoff);
    offset += 96;
    ++spotLightCount;
  }

  glBufferSubData(buffer, 0, sizeof(unsigned int), &directionalLightCount);
  glBufferSubData(buffer, 4, sizeof(unsigned int), &pointLightCount);
  glBufferSubData(buffer, 8, sizeof(unsigned int), &spotLightCount);
  glBindBuffer(buffer, 0);
}

void InitializeShadowUniformBuffer(
  const World::Space& space, Renderable::Collection* collection)
{
  // Determine whether the shadow exists.
  GLenum buffer = GL_UNIFORM_BUFFER;
  glBindBuffer(buffer, nShadowUniformBufferVbo);
  Ds::Vector<World::MemberId> slice = space.Slice<Comp::ShadowMap>();
  bool shadowExists = slice.Size() == 0 ? false : true;
  glBufferSubData(buffer, 0, sizeof(GLuint), &shadowExists);
  if (!shadowExists) {
    glBindBuffer(buffer, 0);
    return;
  }

  // Set the shadow bias and proj view matrix.
  Comp::ShadowMap& shadowMap = space.Get<Comp::ShadowMap>(slice[0]);
  glBufferSubData(buffer, 4, sizeof(GLfloat), &shadowMap.mBias);

  Comp::Transform& transform = space.Get<Comp::Transform>(slice[0]);
  Comp::Camera& camera = space.Get<Comp::Camera>(slice[0]);
  World::Object owner(const_cast<World::Space*>(&space), slice[0]);
  Mat4 view = transform.GetInverseWorldMatrix(owner);
  Mat4 proj = camera.Proj();
  Mat4 projView = proj * view;
  Mat4 projViewTranspose = Math::Transpose(projView);
  glBufferSubData(buffer, 16, sizeof(Mat4), projViewTranspose.mD);
  glBindBuffer(buffer, 0);

  // Render the depth of all renderables to the shadow texture and add that
  // texture to the collection uniforms.
  glViewport(0, 0, shadowMap.mWidth, shadowMap.mHeight);
  glBindFramebuffer(GL_FRAMEBUFFER, shadowMap.mFbo);
  glClear(GL_DEPTH_BUFFER_BIT);
  Gfx::Shader& depthShader = Rsl::GetRes<Shader>(nDepthShaderId);
  depthShader.Use();
  depthShader.SetUniform("uProjView", projView);
  const Ds::Vector<Renderable>& renderables =
    collection->Get(Renderable::Type::Floater);
  for (const Renderable& renderable : renderables) {
    const Mesh* mesh = Rsl::TryGetRes<Mesh>(renderable.mMeshId);
    if (mesh == nullptr) {
      continue;
    }
    depthShader.SetUniform("uModel", renderable.mTransform);
    mesh->Render();
  }
  collection->mUniforms.Add<GLuint>(
    UniformTypeId::Texture2d, "uShadowTexture", shadowMap.mTbo);
}

void RenderMemberIds(
  const World::Space& space, const Mat4& view, const Mat4& proj)
{
  Renderable::Collection collection;
  collection.Collect(space);
  InitializeUniversalUniformBuffer(view, proj);

  auto& memberIdShader = Rsl::GetRes<Gfx::Shader>(nMemberIdShaderId);
  memberIdShader.Use();
  const Ds::Vector<Renderable>& floaters =
    collection.Get(Renderable::Type::Floater);
  for (const Renderable& renderable : floaters) {
    const auto* mesh = Rsl::TryGetRes<Gfx::Mesh>(renderable.mMeshId);
    if (mesh == nullptr) {
      continue;
    }
    memberIdShader.SetUniform("uMemberId", renderable.mOwner);
    memberIdShader.SetUniform("uModel", renderable.mTransform);
    mesh->Render();
  }
}

World::MemberId HoveredMemberId(
  const World::Space& space, const Mat4& view, const Mat4& proj)
{
  // Render all of the MemberIds to a framebuffer.
  Framebuffer::Options options;
  options.mWidth = Viewport::Width();
  options.mHeight = Viewport::Height();
  options.mInternalFormat = GL_R32I;
  options.mFormat = GL_RED_INTEGER;
  options.mPixelType = GL_INT;
  Framebuffer handlebuffer(options);
  glBindFramebuffer(GL_FRAMEBUFFER, handlebuffer.Fbo());
  glClearBufferiv(GL_COLOR, 0, &World::nInvalidMemberId);
  glClear(GL_DEPTH_BUFFER_BIT);
  RenderMemberIds(space, view, proj);

  // Find the MemberId at the mouse position.
  World::MemberId memberId;
  const Vec2& mousePos = Input::MousePosition();
  glReadPixels(
    (int)mousePos[0],
    Viewport::Height() - (int)mousePos[1],
    1,
    1,
    handlebuffer.Format(),
    handlebuffer.PixelType(),
    (void*)&memberId);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return memberId;
}

void RenderSpace(
  const World::Space& space,
  const Mat4& view,
  const Mat4& proj,
  const Vec3& viewPos,
  const ResId& postMaterialId)
{
  // Render only if the post process material and shader are available.
  Material* postMaterial = Rsl::TryGetRes<Material>(postMaterialId);
  if (postMaterial == nullptr) {
    return;
  }
  Shader* postShader = Rsl::TryGetRes<Shader>(postMaterial->mShaderId);
  if (postShader == nullptr) {
    return;
  }
  Renderable::Collection collection;
  collection.Collect(space);

  InitializeUniversalUniformBuffer(view, proj, viewPos);
  InitializeLightsUniformBuffer(space);
  InitializeShadowUniformBuffer(space, &collection);

  // Get the next space framebuffer that hasn't been rendered to and bind it.
  if (nCurrentSpaceFramebufferIndex >= nSpaceFramebuffers.Size()) {
    Framebuffer::Options options;
    options.mWidth = Viewport::Width();
    options.mHeight = Viewport::Height();
    options.mInternalFormat = GL_RGBA16F;
    options.mFormat = GL_RGBA;
    options.mPixelType = GL_HALF_FLOAT;
    nSpaceFramebuffers.Emplace(options);
  }
  const Framebuffer& currentLayerFramebuffer =
    nSpaceFramebuffers[nCurrentSpaceFramebufferIndex];
  ++nCurrentSpaceFramebufferIndex;
  glViewport(0, 0, Viewport::Width(), Viewport::Height());
  glBindFramebuffer(GL_FRAMEBUFFER, currentLayerFramebuffer.Fbo());

  // Perform the render passes for the layer.
  glDepthMask(GL_FALSE);
  collection.Render(Renderable::Type::Skybox);
  glDepthMask(GL_TRUE);
  collection.Render(Renderable::Type::Floater);

  // Perform the post process pass.
  glDisable(GL_DEPTH_TEST);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, currentLayerFramebuffer.ColorTbo());
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  auto& fullscreenMesh = Rsl::GetRes<Gfx::Mesh>(nFullscreenMeshId);
  postShader->Use();
  postShader->SetUniform("uTexture", 0);
  int textureIndex = 1;
  postMaterial->mUniforms.Bind(*postShader, &textureIndex);
  fullscreenMesh.Render();
  glBindTexture(GL_TEXTURE_2D, 0);
  glEnable(GL_DEPTH_TEST);
}

void ResizeSpaceFramebuffers()
{
  for (Gfx::Framebuffer& framebuffer : nSpaceFramebuffers) {
    framebuffer.Resize(Viewport::Width(), Viewport::Height());
  }
}

} // namespace Renderer
} // namespace Gfx
