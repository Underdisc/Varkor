#ifndef Viewport_h
#define Viewport_h

#include "math/Matrix4.h"

class GLFWwindow;

namespace Viewport {

void Init();
void Update();
void Purge();
void SwapBuffers();
const Mat4& Perspective();
GLFWwindow* Window();
bool Active();

} // namespace Viewport

#endif
