#ifndef dbg_draw_h
#define dbg_draw_h

#include "../math/matrix.hh"
#include "../math/vector.hh"

namespace Debug {
namespace Draw {

void Init();
void Line(const Vec3& a, const Vec3& b, const Vec3& color);
void Render(const Mat4& view, const Mat4& proj);

} // namespace Draw
} // namespace Debug

#endif
