#include <GLFW/VarkorGlfw.h>

#include "Viewport.h"
#include "gfx/Framebuffer.h"
#include "math/Constants.h"

namespace Viewport {

void ResizeCallback(GLFWwindow* window, int width, int height);

GLFWwindow* nWindow;
GLFWwindow* nSharedWindow;
bool nActive = true;
float nFov = Math::nPi / 2.0f;
int nWidth;
int nHeight;
float nNear = 0.1f;
float nFar = 1000000.0f;
Mat4 nPerspective;

void Init(const char* windowName, bool visible)
{
  // Create a maximized window and an opengl context.
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
  glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
  if (!visible)
  {
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  }
  // We use 1 for the starting window width and height because it will be
  // maximized to fill the whole monitor.
  nWindow = glfwCreateWindow(800, 800, windowName, NULL, NULL);
  LogAbortIf(!nWindow, "glfw window creation failed.");
  glfwGetWindowSize(nWindow, &nWidth, &nHeight);
  glfwMakeContextCurrent(nWindow);
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  nSharedWindow = glfwCreateWindow(1, 1, "SharedContext", NULL, nWindow);

  bool gladLoaded = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  LogAbortIf(!gladLoaded, "glad initialization failed.");
  glEnable(GL_MULTISAMPLE);

  ResizeCallback(nWindow, nWidth, nHeight);
  glfwSetFramebufferSizeCallback(nWindow, ResizeCallback);
}

void Update()
{
  nActive = !glfwWindowShouldClose(nWindow);
}

void Purge()
{
  glfwDestroyWindow(nSharedWindow);
  glfwDestroyWindow(nWindow);
  glfwTerminate();
}

void SwapBuffers()
{
  glfwSwapBuffers(nWindow);
}

void InitContextSharing()
{
  glfwMakeContextCurrent(nSharedWindow);
}

const Mat4& Perspective()
{
  return nPerspective;
}

Vec3 MouseToWorldPosition(Vec2 standardPosition, const Mat4& inverseView)
{
  // The value returned by this function will be on the camera frustrum's near
  // plane.
  float heightOver2 = std::tanf(nFov / 2.0f) * nNear;
  standardPosition[0] *= heightOver2 * Aspect();
  standardPosition[1] *= heightOver2;
  Vec4 worldPosition = {standardPosition[0], standardPosition[1], nNear, 1.0f};
  worldPosition = inverseView * worldPosition;
  return (Vec3)worldPosition;
}

float Near()
{
  return nNear;
}

float Far()
{
  return nFar;
}

int Width()
{
  return nWidth;
}

int Height()
{
  return nHeight;
}

float Aspect()
{
  return (float)nWidth / (float)nHeight;
}

float Fov()
{
  return nFov;
}

GLFWwindow* Window()
{
  return nWindow;
}

bool Active()
{
  return nActive;
}

void ResizeCallback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
  nWidth = width;
  nHeight = height;
  Math::Perspective(&nPerspective, nFov, Aspect(), nNear, nFar);
  Gfx::Framebuffer::ResizeFullscreens(width, height);
}

} // namespace Viewport
