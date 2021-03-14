#include "math/Matrix4.h"
#include "world/Space.h"

namespace Gfx {
namespace Renderer {

void Init();
void Render(World::SpaceId spaceId, const Mat4& view);

} // namespace Renderer
} // namespace Gfx
