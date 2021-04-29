#include "comp/Model.h"
#include "gfx/Framebuffer.h"
#include "gfx/Shader.h"
#include "math/Matrix4.h"
#include "world/Space.h"

namespace Gfx {
namespace Renderer {

void Init();
void Clear();
void RenderModel(const Shader& shader, const Comp::Model& modelComp);
void RenderMemberIds(const World::Space& space, const Mat4& view);
World::MemberId HoveredMemberId(const World::Space& space, const Mat4& view);
void RenderSpace(const World::Space& space, const Mat4& view);
void RenderWorld();
void RenderFullscreenTexture(unsigned int tbo);
void RenderQueuedFullscreenFramebuffers();
void QueueFullscreenFramebuffer(const Framebuffer& framebuffer);

const Shader& ColorShader();

} // namespace Renderer
} // namespace Gfx
