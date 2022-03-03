#include "Result.h"
#include "comp/Model.h"
#include "gfx/Framebuffer.h"
#include "gfx/Shader.h"
#include "math/Matrix4.h"
#include "world/Space.h"

namespace Gfx {
namespace Renderer {

void Init();
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
void RenderFramebuffers();
void QueueFullscreenFramebuffer(const Framebuffer& framebuffer);

} // namespace Renderer
} // namespace Gfx
