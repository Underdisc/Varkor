#ifndef debug_Draw_h
#define debug_Draw_h

#include "math/Geometry.h"

namespace Debug {
namespace Draw {

void Init();
void Point(const Vec3& point, const Vec3& color);
void Line(const Vec3& a, const Vec3& b, const Vec3& color);
void Plane(const Math::Plane& plane, const Vec3& color);
void CartesianAxes();
void Render(const Mat4& view, const Mat4& proj);

} // namespace Draw
} // namespace Debug

#endif
