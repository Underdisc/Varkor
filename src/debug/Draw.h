#ifndef debug_Draw_h
#define debug_Draw_h

namespace Debug {
namespace Draw {

void Init();
void Line(const Vec3& a, const Vec3& b, const Vec3& color);
void CartesianAxes();
void Render(const Mat4& view, const Mat4& proj);

} // namespace Draw
} // namespace Debug

#endif
