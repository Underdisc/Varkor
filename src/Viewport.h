#ifndef Viewport_h
#define Viewport_h

struct GLFWwindow;

namespace Viewport {

void Init(const char* windowName, bool visible = true);
void Update();
void Purge();
void SwapBuffers();
void InitContextSharing();

int Width();
int Height();
float Aspect();
GLFWwindow* Window();
bool Active();

} // namespace Viewport

#endif
