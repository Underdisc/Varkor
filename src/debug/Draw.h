#ifndef debug_Draw_h
#define debug_Draw_h

#include "gfx/Renderable.h"
#include "math/Geometry.h"
#include "math/Matrix4.h"

namespace Debug {
namespace Draw {

void Init();
void Point(const Vec3& point, const Vec3& color);
void Line(const Vec3& a, const Vec3& b, const Vec3& color);
void Line(const Vec3& a, const Vec3& b, const Vec3& aColor, const Vec3& bColor);
void Plane(const Math::Plane& plane, const Vec3& color);
void CartesianAxes();
void RenderTbnVectors(const Gfx::Collection& collection);
void Render(const World::Object& cameraObject);

} // namespace Draw
} // namespace Debug

#endif
