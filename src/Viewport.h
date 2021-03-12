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
float Near();
float Far();
float Aspect();
GLFWwindow* Window();
bool Active();

} // namespace Viewport

#endif
