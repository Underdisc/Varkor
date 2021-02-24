// glad.h must be included before glfw3.h.
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include "math/Constants.h"

#include "Viewport.h"

namespace Viewport {

void ResizeCallback(GLFWwindow* window, int width, int height);

GLFWwindow* nWindow;
bool nActive = true;
float nFov = Math::nPi / 2.0f;
int nWidth = 800;
int nHeight = 800;
float nNear = 0.1f;
float nFar = 100.0f;
Mat4 nPerspective;

void Init(bool visible)
{
  // Create the window and opengl context.
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  if (!visible)
  {
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  }
  nWindow = glfwCreateWindow(nWidth, nHeight, "Varkor", NULL, NULL);
  LogAbortIf(!nWindow, "glfw window creation failed.");
  glfwMakeContextCurrent(nWindow);

  bool gladLoaded = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  LogAbortIf(!gladLoaded, "glad initialization failed.");

  ResizeCallback(nWindow, nWidth, nHeight);
  glfwSetFramebufferSizeCallback(nWindow, ResizeCallback);
}

void Update()
{
  nActive = !glfwWindowShouldClose(nWindow);
}

void Purge()
{
  glfwTerminate();
}

void SwapBuffers()
{
  glfwSwapBuffers(nWindow);
}

const Mat4& Perspective()
{
  return nPerspective;
}

float Near()
{
  return nNear;
}

float Far()
{
  return nFar;
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
  float aspect = (float)width / (float)height;
  Math::Perspective(&nPerspective, nFov, aspect, nNear, nFar);
  nWidth = width;
  nHeight = height;
}

} // namespace Viewport
