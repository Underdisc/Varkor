#include <glad/glad.h>
#include <sstream>
#include <utility>

#include "Input.h"
#include "Temporal.h"
#include "Viewport.h"
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
#include "ext/Tracy.h"
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

int nBloomBlurCount = 3;
float nBloomLuminanceThreshold = 1.0f;
ResId nTonemapMaterial = "vres/renderer:ReinhardTonemap";
int nMemberOutlineWidth = 1;
Vec4 nMemberOutlineColor = {1.0f, 1.0f, 1.0f, 1.0f};
void (*nCustomRender)() = nullptr;

GLuint nUniversalUniformBufferVbo;
GLuint nLightsUniformBufferVbo;
GLuint nShadowUniformBufferVbo;

constexpr GLuint nUnusedFbo = 0;

GLuint nMemberIdFbo = nUnusedFbo;
GLuint nMemberIdColorTbo;
GLuint nMemberIdDepthTbo;
bool nMemberIdFboUsed = false;

const char* nRendererAssetName = "vres/renderer";
const ResId nFullscreenMeshId(nRendererAssetName, "Fullscreen");
const ResId nSpriteMeshId(nRendererAssetName, "Sprite");
const ResId nSkyboxMeshId(nRendererAssetName, "Skybox");
const ResId nMemberIdShaderId(nRendererAssetName, "MemberId");
const ResId nDepthShaderId(nRendererAssetName, "Depth");
const ResId nDefaultPostId(nRendererAssetName, "CopyTexture");

// Required framebuffers
GLuint nLayerFbo;
GLuint nLayerColorTbo;
GLuint nLayerDepthTbo;
GLuint nLayerResolvedFbo;
GLuint nLayerResolvedTbo;
GLuint nBlendedFbo;
GLuint nBlendedTbo;
GLuint nBlurFbos[2];
GLuint nBlurTbos[2];
GLuint nFinalHdrFbo;
GLuint nFinalHdrTbo;

void InitRequiredFramebuffers(int width, int height) {
  // Create the multisample buffer that renderables are rendered to.
  glGenFramebuffers(1, &nLayerFbo);
  glBindFramebuffer(GL_FRAMEBUFFER, nLayerFbo);

  glGenTextures(1, &nLayerColorTbo);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, nLayerColorTbo);
  glTexImage2DMultisample(
    GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA16F, width, height, GL_TRUE);
  glFramebufferTexture2D(
    GL_FRAMEBUFFER,
    GL_COLOR_ATTACHMENT0,
    GL_TEXTURE_2D_MULTISAMPLE,
    nLayerColorTbo,
    0);

  glGenTextures(1, &nLayerDepthTbo);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, nLayerDepthTbo);
  glTexImage2DMultisample(
    GL_TEXTURE_2D_MULTISAMPLE, 4, GL_DEPTH_COMPONENT, width, height, GL_TRUE);
  glFramebufferTexture2D(
    GL_FRAMEBUFFER,
    GL_DEPTH_ATTACHMENT,
    GL_TEXTURE_2D_MULTISAMPLE,
    nLayerDepthTbo,
    0);

  // Generates a half float rgba framebuffer.
  auto genRgba16fFramebuffer = [&width, &height](GLuint* fbo, GLuint* tbo)
  {
    glGenFramebuffers(1, fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, *fbo);
    glGenTextures(1, tbo);
    glBindTexture(GL_TEXTURE_2D, *tbo);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RGBA16F,
      width,
      height,
      0,
      GL_RGBA,
      GL_HALF_FLOAT,
      nullptr);
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *tbo, 0);
  };
  genRgba16fFramebuffer(&nLayerResolvedFbo, &nLayerResolvedTbo);
  genRgba16fFramebuffer(&nBlendedFbo, &nBlendedTbo);
  for (int i = 0; i < 2; ++i) {
    genRgba16fFramebuffer(&nBlurFbos[i], &nBlurTbos[i]);
  }
  genRgba16fFramebuffer(&nFinalHdrFbo, &nFinalHdrTbo);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void PurgeRequiredFramebuffers() {
  // Delete the framebuffer objects.
  glDeleteFramebuffers(1, &nLayerFbo);
  glDeleteFramebuffers(1, &nLayerResolvedFbo);
  glDeleteFramebuffers(1, &nBlendedFbo);
  for (int i = 0; i < 2; ++i) {
    glDeleteFramebuffers(1, &nBlurFbos[i]);
  }
  glDeleteFramebuffers(1, &nFinalHdrFbo);

  // Delete the texture objects.
  glDeleteTextures(1, &nLayerColorTbo);
  glDeleteTextures(1, &nLayerDepthTbo);
  glDeleteTextures(1, &nLayerResolvedTbo);
  glDeleteTextures(1, &nBlendedTbo);
  for (int i = 0; i < 2; ++i) {
    glDeleteTextures(1, &nBlurTbos[i]);
  }
  glDeleteTextures(1, &nFinalHdrTbo);
}

void PurgeMemberIdFbo() {
  if (nMemberIdFbo != nUnusedFbo) {
    glDeleteFramebuffers(1, &nMemberIdFbo);
    glDeleteTextures(1, &nMemberIdColorTbo);
    glDeleteTextures(1, &nMemberIdDepthTbo);
  }
  nMemberIdFbo = nUnusedFbo;
}

void Init() {
  // Initialize all of the resources used by the renderer.
  Rsl::Asset& asset = Rsl::AddAsset(nRendererAssetName);
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
  CreateUniformBuffer(&nLightsUniformBufferVbo, 17'680, 1);
  CreateUniformBuffer(&nShadowUniformBufferVbo, 80, 2);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  InitRequiredFramebuffers(Viewport::Width(), Viewport::Height());
}

void Purge() {
  PurgeRequiredFramebuffers();
  PurgeMemberIdFbo();
}

void Clear() {
  glBindFramebuffer(GL_FRAMEBUFFER, nBlendedFbo);
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_DEPTH_BUFFER_BIT);

  // Delete the MemberId buffer if it wasn't used during the last frame.
  if (!nMemberIdFboUsed) {
    PurgeMemberIdFbo();
  }
  nMemberIdFboUsed = false;
}

void ResizeRequiredFramebuffers() {
  PurgeRequiredFramebuffers();
  InitRequiredFramebuffers(Viewport::Width(), Viewport::Height());
  PurgeMemberIdFbo();
}

void InitializeUniversalUniformBuffer(const World::Object& cameraObject) {
  const auto& cameraComp = cameraObject.Get<Comp::Camera>();
  Mat4 viewTranspose = Math::Transpose(cameraComp.View(cameraObject));
  Mat4 projTranspose = Math::Transpose(cameraComp.Proj());
  auto& transformComp = cameraObject.Get<Comp::Transform>();
  Vec3 viewPos = transformComp.GetWorldTranslation(cameraObject);
  float totalTime = Temporal::TotalTime();
  glBindBuffer(GL_UNIFORM_BUFFER, nUniversalUniformBufferVbo);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Mat4), viewTranspose.CData());
  glBufferSubData(GL_UNIFORM_BUFFER, 64, sizeof(Mat4), projTranspose.CData());
  glBufferSubData(GL_UNIFORM_BUFFER, 128, sizeof(Vec3), viewPos.mD);
  glBufferSubData(GL_UNIFORM_BUFFER, 140, sizeof(float), &totalTime);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void InitializeLightsUniformBuffer(const World::Space& space) {
  GLenum buffer = GL_UNIFORM_BUFFER;
  glBindBuffer(buffer, nLightsUniformBufferVbo);

  const unsigned int maxDirectionalLights = 1;
  unsigned int directionalLightCount = 0;
  GLintptr offset = 16;
  Ds::Vector<World::MemberId> slice = space.Slice<Comp::DirectionalLight>();
  for (int i = 0; i < slice.Size() && i < maxDirectionalLights; ++i) {
    World::Object owner(const_cast<World::Space*>(&space), slice[i]);
    auto& transform = space.Get<Comp::Transform>(slice[i]);
    auto& light = space.Get<Comp::DirectionalLight>(slice[i]);
    Vec3 direction =
      transform.GetWorldRotation(owner).Rotate({1.0f, 0.0f, 0.0f});
    Vec3 trueAmbient = light.mAmbient.TrueColor();
    Vec3 trueDiffuse = light.mDiffuse.TrueColor();
    Vec3 trueSpecular = light.mSpecular.TrueColor();
    glBufferSubData(buffer, offset, sizeof(Vec3), direction.mD);
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
    World::Object owner(const_cast<World::Space*>(&space), slice[i]);
    auto& transform = space.Get<Comp::Transform>(slice[i]);
    auto& light = space.Get<Comp::PointLight>(slice[i]);
    Vec3 translation = transform.GetWorldTranslation(owner);
    Vec3 trueAmbient = light.mAmbient.TrueColor();
    Vec3 trueDiffuse = light.mDiffuse.TrueColor();
    Vec3 trueSpecular = light.mSpecular.TrueColor();
    glBufferSubData(buffer, offset, sizeof(Vec3), translation.mD);
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
    World::Object owner(const_cast<World::Space*>(&space), slice[i]);
    auto& transform = space.Get<Comp::Transform>(slice[i]);
    auto& light = space.Get<Comp::SpotLight>(slice[i]);
    Vec3 translation = transform.GetWorldTranslation(owner);
    Quat rotation = transform.GetWorldRotation(owner);
    Vec3 direction = rotation.Rotate({1.0f, 0.0f, 0.0f});
    Vec3 trueAmbient = light.mAmbient.TrueColor();
    Vec3 trueDiffuse = light.mDiffuse.TrueColor();
    Vec3 trueSpecular = light.mSpecular.TrueColor();
    glBufferSubData(buffer, offset, sizeof(Vec3), translation.mD);
    glBufferSubData(buffer, offset + 16, sizeof(Vec3), direction.mD);
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
  const World::Space& space, Collection* collection) {
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
  for (const Renderable::Floater& floater: collection->mFloaters) {
    const Mesh* mesh = Rsl::TryGetRes<Mesh>(floater.mMeshId);
    if (mesh == nullptr) {
      continue;
    }
    depthShader.SetUniform("uModel", floater.mTransform);
    mesh->Render();
  }
  collection->mUniforms.Add<GLuint>(
    UniformTypeId::Texture2d, "uShadowTexture", shadowMap.mTbo);
}

void EnsureMemberIdFbo() {
  nMemberIdFboUsed = true;
  if (nMemberIdFbo != nUnusedFbo) {
    return;
  }
  glGenFramebuffers(1, &nMemberIdFbo);
  glBindFramebuffer(GL_FRAMEBUFFER, nMemberIdFbo);

  glGenTextures(1, &nMemberIdColorTbo);
  glBindTexture(GL_TEXTURE_2D, nMemberIdColorTbo);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    GL_R32I,
    Viewport::Width(),
    Viewport::Height(),
    0,
    GL_RED_INTEGER,
    GL_INT,
    nullptr);
  glFramebufferTexture2D(
    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nMemberIdColorTbo, 0);

  glGenTextures(1, &nMemberIdDepthTbo);
  glBindTexture(GL_TEXTURE_2D, nMemberIdDepthTbo);
  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    GL_DEPTH_COMPONENT,
    Viewport::Width(),
    Viewport::Height(),
    0,
    GL_DEPTH_COMPONENT,
    GL_UNSIGNED_INT,
    nullptr);
  glFramebufferTexture2D(
    GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, nMemberIdDepthTbo, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderMemberIds(
  const Collection& collection, const World::Object& cameraObject) {
  InitializeUniversalUniformBuffer(cameraObject);
  auto& memberIdShader = Rsl::GetRes<Gfx::Shader>(nMemberIdShaderId);
  memberIdShader.Use();
  for (const Renderable::Floater& floater: collection.mFloaters) {
    const auto* mesh = Rsl::TryGetRes<Gfx::Mesh>(floater.mMeshId);
    if (mesh == nullptr) {
      continue;
    }
    memberIdShader.SetUniform("uMemberId", floater.mOwner);
    memberIdShader.SetUniform("uModel", floater.mTransform);
    mesh->Render();
  }
  collection.RenderIcons(true, cameraObject);
}

void RenderMemberOutline(
  const Collection& collection, const World::Object& cameraObject) {
  // Render the memberIds.
  EnsureMemberIdFbo();
  glBindFramebuffer(GL_FRAMEBUFFER, nMemberIdFbo);
  glClearBufferiv(GL_COLOR, 0, &World::nInvalidMemberId);
  glClear(GL_DEPTH_BUFFER_BIT);
  RenderMemberIds(collection, cameraObject);

  // Draw an outline around the valid memberIds to the default framebuffer.
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, nMemberIdColorTbo);
  auto& outlineShader = Rsl::GetRes<Gfx::Shader>("vres/renderer:MemberOutline");
  outlineShader.Use();
  outlineShader.SetUniform("uTexture", 0);
  outlineShader.SetUniform("uWidth", nMemberOutlineWidth);
  outlineShader.SetUniform("uColor", nMemberOutlineColor);

  auto& fullscreenMesh = Rsl::GetRes<Gfx::Mesh>(nFullscreenMeshId);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  fullscreenMesh.Render();
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
}

World::MemberId HoveredMemberId(
  const World::Space& space, const World::Object& cameraObject) {
  // Render all of the MemberIds to a framebuffer.
  Collection collection;
  collection.Collect(space);
  EnsureMemberIdFbo();
  glBindFramebuffer(GL_FRAMEBUFFER, nMemberIdFbo);
  glClearBufferiv(GL_COLOR, 0, &World::nInvalidMemberId);
  glClear(GL_DEPTH_BUFFER_BIT);
  RenderMemberIds(collection, cameraObject);

  // Find the MemberId at the mouse position.
  World::MemberId memberId;
  const Vec2& mousePos = Input::MousePosition();
  glReadPixels(
    (int)mousePos[0],
    Viewport::Height() - (int)mousePos[1],
    1,
    1,
    GL_RED_INTEGER,
    GL_INT,
    (void*)&memberId);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return memberId;
}

void RenderLayer(const World::Space& space, const World::Object& cameraObject) {
  Collection collection;
  collection.Collect(space);

  InitializeUniversalUniformBuffer(cameraObject);
  InitializeLightsUniformBuffer(space);
  InitializeShadowUniformBuffer(space, &collection);

  // Render all renderables to the multisample buffer.
  glViewport(0, 0, Viewport::Width(), Viewport::Height());
  glBindFramebuffer(GL_FRAMEBUFFER, nLayerFbo);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if (collection.HasSkybox()) {
    glDepthMask(GL_FALSE);
    collection.RenderSkybox();
    glDepthMask(GL_TRUE);
  }
  collection.RenderFloaters();
  collection.RenderIcons(false, cameraObject);

  // Resolve the multisampled layer buffer.
  glBindFramebuffer(GL_READ_FRAMEBUFFER, nLayerFbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, nLayerResolvedFbo);
  glBlitFramebuffer(
    0,
    0,
    Viewport::Width(),
    Viewport::Height(),
    0,
    0,
    Viewport::Width(),
    Viewport::Height(),
    GL_COLOR_BUFFER_BIT,
    GL_NEAREST);
}

void BlendLayer(GLuint toFbo, const ResId& postMaterialId) {
  // Acquire the material.
  const auto* material =
    Rsl::TryGetRes<Material>(postMaterialId, nDefaultPostId);
  if (material == nullptr) {
    return;
  }
  const auto* shader =
    Rsl::TryGetRes<Shader>(material->mShaderId, nDefaultPostId);
  if (shader == nullptr) {
    return;
  }
  shader->Use();
  shader->SetUniform("uTexture", 0);
  int textureIndex = 1;
  material->Bind(*shader, &textureIndex);

  // Blend the layer.
  glBindFramebuffer(GL_FRAMEBUFFER, toFbo);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, nLayerResolvedTbo);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  auto& fullscreenMesh = Rsl::GetRes<Mesh>("vres/renderer:Fullscreen");
  fullscreenMesh.Render();
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
}

void BloomAndTonemapPasses() {
  // Extract the instense colors into the first blur buffer.
  glDisable(GL_DEPTH_TEST);
  glBindFramebuffer(GL_FRAMEBUFFER, nBlurFbos[0]);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, nBlendedTbo);
  auto& fullscreenMesh = Rsl::GetRes<Mesh>("vres/renderer:Fullscreen");
  auto& intenseExtractShader =
    Rsl::GetRes<Shader>("vres/renderer:IntenseExtract");
  intenseExtractShader.Use();
  intenseExtractShader.SetUniform(
    "uLuminanceThreshold", nBloomLuminanceThreshold);
  fullscreenMesh.Render();

  // Perform the blur passes.
  auto& blurShader = Rsl::GetRes<Shader>("vres/renderer:Blur");
  blurShader.Use();
  for (int i = 0; i < nBloomBlurCount; ++i) {
    // Horizontal
    glBindFramebuffer(GL_FRAMEBUFFER, nBlurFbos[1]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, nBlurTbos[0]);
    blurShader.SetUniform("uTexture", 0);
    blurShader.SetUniform("uDirection", Vec2({1.0f, 0.0f}));
    fullscreenMesh.Render();

    // Vertical
    glBindFramebuffer(GL_FRAMEBUFFER, nBlurFbos[0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, nBlurTbos[1]);
    blurShader.SetUniform("uDirection", Vec2({0.0f, 1.0f}));
    fullscreenMesh.Render();
  }

  // Blend the blurred intense colors with the main color.
  glBindFramebuffer(GL_FRAMEBUFFER, nFinalHdrFbo);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, nBlendedTbo);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, nBlurTbos[0]);
  auto& additiveBlendShader =
    Rsl::GetRes<Shader>("vres/renderer:AdditiveBlend");
  additiveBlendShader.Use();
  additiveBlendShader.SetUniform("uMain", 0);
  additiveBlendShader.SetUniform("uOther", 1);
  fullscreenMesh.Render();

  // Acquire the tonemapping material.
  const auto* tonemapMaterial = Rsl::TryGetRes<Material>(
    nTonemapMaterial, "vres/renderer:FullscreenDefault");
  if (tonemapMaterial == nullptr) {
    return;
  }
  const auto* tonemapShader = Rsl::TryGetRes<Shader>(
    tonemapMaterial->mShaderId, "vres/renderer:FullscreenDefault");
  if (tonemapShader == nullptr) {
    return;
  }
  tonemapShader->Use();
  tonemapShader->SetUniform("uTexture", 0);
  int textureIndex = 1;
  tonemapMaterial->Bind(*tonemapShader, &textureIndex);

  // Perform tonemapping into the default framebuffer.
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, nFinalHdrTbo);
  fullscreenMesh.Render();
  glEnable(GL_DEPTH_TEST);
}

Result RenderWorld() {
  for (World::Layer& layer: World::nLayers) {
    // Make sure the layer has a camera.
    if (layer.mCameraId == World::nInvalidMemberId) {
      std::stringstream error;
      error << "\"" << layer.mName << "\" layer has no assigned camera.";
      return Result(error.str());
    }
    // Make sure the camera has a camera component.
    World::Space& space = layer.mSpace;
    const auto* cameraComp = space.TryGet<Comp::Camera>(layer.mCameraId);
    if (cameraComp == nullptr) {
      std::stringstream error;
      error << "\"" << layer.mName << "\" layer camera has no camera component";
      return Result(error.str());
    }
    const World::Object cameraObject(&space, layer.mCameraId);

    // Render the space using the layer camera.
    RenderLayer(space, cameraObject);
    BlendLayer(nBlendedFbo, "vres/renderer:CopyTexture");
    BloomAndTonemapPasses();
    Debug::Draw::Render(cameraObject);
  }
  return Result();
}

void Render() {
  ZoneScoped;

  Clear();
  if (nCustomRender != nullptr) {
    nCustomRender();
  }
  if (!Editor::nPlayMode) {
    // Render the selected space.
    Editor::LayerInterface* layerInterface =
      Editor::nCoreInterface.FindInterface<Editor::LayerInterface>();
    const World::Object cameraObject = Editor::nCamera.GetObject();
    if (layerInterface != nullptr) {
      const World::Layer& worldLayer = *layerInterface->mLayerIt;
      const World::Space& space = worldLayer.mSpace;

      RenderLayer(space, cameraObject);
      BlendLayer(nBlendedFbo, worldLayer.mPostMaterialId);
      BloomAndTonemapPasses();

      // Render an outline around the selected object.
      Editor::InspectorInterface* inspectorInterface =
        layerInterface->FindInterface<Editor::InspectorInterface>();
      if (inspectorInterface != nullptr) {
        Collection collection;
        collection.Collect(inspectorInterface->mObject);
        RenderMemberOutline(collection, cameraObject);
      }
    }

    // Render the editor space and any debug draws.
    RenderLayer(Editor::nSpace, cameraObject);
    BlendLayer(0, "vres/renderer:CopyTexture");
    Debug::Draw::Render(cameraObject);
  }
  else {
    Result result = RenderWorld();
    if (!result.Success()) {
      LogError(result.mError.c_str());
      Editor::nPlayMode = false;
    }
  }
}

} // namespace Renderer
} // namespace Gfx
