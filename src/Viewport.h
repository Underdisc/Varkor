#ifndef Viewport_h
#define Viewport_h

#include "math/Matrix4.h"

struct GLFWwindow;

namespace Viewport {

void Init(bool visible = true);
void Update();
void Purge();
void SwapBuffers();

const Mat4& Perspective();
float Near();
float Far();
GLFWwindow* Window();
bool Active();

} // namespace Viewport

#endif
