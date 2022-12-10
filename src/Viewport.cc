#include <GLFW/VarkorGlfw.h>

#include "Viewport.h"
#include "gfx/Renderer.h"

namespace Viewport {

void ResizeCallback(GLFWwindow* window, int width, int height);

GLFWwindow* nWindow;
GLFWwindow* nSharedWindow;
bool nActive = true;
int nWidth;
int nHeight;

void Init(const char* windowName, bool visible)
{
  // Create a maximized window and an opengl context.
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
  glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
  if (!visible) {
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
  nWidth = width;
  nHeight = height;
  Gfx::Renderer::LayerFramebuffers::Resize();
}

} // namespace Viewport
