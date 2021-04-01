#include "math/Matrix4.h"
#include "world/Space.h"

namespace Gfx {
namespace Renderer {

void Init();
void Clear();
void RenderMemberIds(const World::Space& space, const Mat4& view);
void RenderSpace(const World::Space& space, const Mat4& view);
void RenderWorld();

} // namespace Renderer
} // namespace Gfx
