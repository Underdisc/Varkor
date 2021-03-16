#include "math/Matrix4.h"
#include "world/Space.h"

namespace Gfx {
namespace Renderer {

void Init();
void Clear();
void RenderSpace(World::SpaceId spaceId, const Mat4& view);
void RenderWorld();

} // namespace Renderer
} // namespace Gfx
