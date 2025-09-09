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
extern const ResId nDefaultPostId;
extern const ResId nDefaultIntenseExtractId;
extern const ResId nDefaultTonemapId;

void Init();
void Purge();
void Clear();
void ResizeRequiredFramebuffers();

World::MemberId HoveredMemberId(
  const World::Space& space, const World::Object& cameraObject);

void RenderLayer(const World::Space& space, const World::Object& cameraObject);
Result RenderWorld();
void Render();

// todo: This really shouldn't be exposed by the Renderer.
void InitializeUniversalUniformBuffer(const World::Object& cameraObject);

} // namespace Renderer
} // namespace Gfx

#endif
