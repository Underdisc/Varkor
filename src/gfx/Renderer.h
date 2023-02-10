#ifndef gfx_Renderer_h
#define gfx_Renderer_h

#include <glad/glad.h>

#include "Result.h"
#include "gfx/Framebuffer.h"
#include "gfx/Shader.h"
#include "math/Matrix4.h"
#include "world/Space.h"

namespace Gfx {
namespace Renderer {

extern int nBloomBlurCount;
extern float nBloomLuminanceThreshold;
extern ResId nTonemapMaterial;

extern void (*nCustomRender)();

extern const ResId nFullscreenMeshId;
extern const ResId nSpriteMeshId;
extern const ResId nSkyboxMeshId;
extern const ResId nDefaultPostMaterialId;

void Init();
void Purge();
void Clear();
void Render();

World::MemberId HoveredMemberId(
  const World::Space& space, const Mat4& view, const Mat4& proj);
void RenderLayer(
  const World::Space& space,
  const Mat4& view,
  const Mat4& proj,
  const Vec3& viewPos);
Result RenderWorld();

void ResizeRequiredFramebuffers();

// todo: This really shouldn't be exposed by the Renderer.
void InitializeUniversalUniformBuffer(
  const Mat4& view, const Mat4& proj, const Vec3& viewPos = {0.0f, 0.0f, 0.0f});

} // namespace Renderer
} // namespace Gfx

#endif