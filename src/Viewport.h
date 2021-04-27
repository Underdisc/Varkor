#ifndef Viewport_h
#define Viewport_h

#include "math/Matrix4.h"

struct GLFWwindow;

namespace Viewport {

void Init(const char* windowName, bool visible = true);
void Update();
void Purge();
void SwapBuffers();

const Mat4& Perspective();
Vec3 MouseToWorldPosition(Vec2 standardPosition, const Mat4& inverseView);
float Near();
float Far();
int Width();
int Height();
float Aspect();
float Fov();
GLFWwindow* Window();
bool Active();

} // namespace Viewport

#endif
