#ifndef gfx_Renderer_h
#define gfx_Renderer_h

#include <glad/glad.h>

#include "Result.h"
#include "math/Matrix4.h"
#include "world/Space.h"

namespace Gfx::Renderer {

extern GLuint nSpriteVao;
extern GLuint nSpriteVbo;

extern GLuint nSkyboxVao;
extern GLsizei nSkyboxElementCount;

extern void (*nCustomRender)();

void Init();
void Purge();
void Clear();
void Render();
void RenderMemberIds(
  const World::Space& space, const Mat4& view, const Mat4& proj);
World::MemberId HoveredMemberId(
  const World::Space& space, const Mat4& view, const Mat4& proj);
void RenderSpace(
  const World::Space& space,
  const Mat4& view,
  const Mat4& proj,
  const Vec3& viewPos);
Result RenderWorld();
void RenderQuad(GLuint vao);
void RenderFramebuffers();

// todo: This really shouldn't be exposed by the Renderer.
void InitializeUniversalUniformBuffer(
  const Mat4& view, const Mat4& proj, const Vec3& viewPos = {0.0f, 0.0f, 0.0f});

} // namespace Gfx::Renderer

#endif