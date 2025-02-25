#ifndef debug_Draw_h
#define debug_Draw_h

#include "gfx/Renderable.h"
#include "math/Box.h"
#include "math/Capsule.h"
#include "math/Matrix4.h"
#include "math/Plane.h"
#include "math/Sphere.h"
#include "math/Triangle.h"

namespace Debug {
namespace Draw {

void Init();
void Point(const Vec3& point, const Vec3& color);
void Line(const Vec3& a, const Vec3& b, const Vec3& color);
void Line(const Vec3& a, const Vec3& b, const Vec3& aColor, const Vec3& bColor);
void Plane(const Math::Plane& plane, const Vec3& color);
void Box(const Math::Box& box, const Vec3& color);
void Sphere(const Math::Sphere& sphere, const Vec3& color);
void Capsule(const Math::Capsule& capsule, const Vec3& color);
void Triangle(const Math::Triangle& triangle, const Vec3& color);
void CartesianAxes();
void RenderTbnVectors(const Gfx::Collection& collection);
void Render(const World::Object& cameraObject);

} // namespace Draw
} // namespace Debug

#endif
