#include <GLFW/VarkorGlfw.h>

#include "Viewport.h"
#include "gfx/Renderer.h"

namespace Viewport {

void ResizeCallback(GLFWwindow* window, int width, int height);
const char* ErrorCodeString(int code);
void ErrorCallback(int code, const char* description);

GLFWwindow* nWindow;
GLFWwindow* nSharedWindow;
bool nActive = true;
int nWidth;
int nHeight;

void Init(const char* windowName, bool visible)
{
  // Create a maximized window and an opengl context.
  glfwSetErrorCallback(ErrorCallback);
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
  LogAbortIf(nWindow == nullptr, "glfw window creation failed.");
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

void StartContextSharing()
{
  glfwMakeContextCurrent(nSharedWindow);
}

void EndContextSharing()
{
  glfwMakeContextCurrent(nullptr);
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

const char* ErrorCodeString(int code)
{
  switch (code) {
  case GLFW_NO_ERROR: return "GLFW_NO_ERROR";
  case GLFW_NOT_INITIALIZED: return "GLFW_NOT_INITIALIZED";
  case GLFW_NO_CURRENT_CONTEXT: return "GLFW_NO_CURRENT_CONTEXT";
  case GLFW_INVALID_ENUM: return "GLFW_INVALID_ENUM";
  case GLFW_INVALID_VALUE: return "GLFW_INVALID_VALUE";
  case GLFW_OUT_OF_MEMORY: return "GLFW_OUT_OF_MEMORY";
  case GLFW_API_UNAVAILABLE: return "GLFW_API_UNAVAILABLE";
  case GLFW_VERSION_UNAVAILABLE: return "GLFW_VERSION_UNAVAILABLE";
  case GLFW_PLATFORM_ERROR: return "GLFW_PLATFORM_ERROR";
  case GLFW_FORMAT_UNAVAILABLE: return "GLFW_FORMAT_UNAVAILABLE";
  case GLFW_NO_WINDOW_CONTEXT: return "GLFW_NO_WINDOW_CONTEXT";
  }
  return "GLFW_NO_ERROR";
}

void ErrorCallback(int code, const char* description)
{
  std::string error = "GlfwError(";
  error += ErrorCodeString(code);
  error += ")\n  ";
  error += description;
  Error::LogString(error.c_str());
  Error::StackTrace();
}

} // namespace Viewport
