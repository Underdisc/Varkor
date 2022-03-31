#include "Input.h"
#include "Viewport.h"
#include "ds/Vector.h"
#include "math/Vector.h"

namespace Input {

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void KeyCallback(
  GLFWwindow* window, int key, int scancode, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xPos, double yPos);
void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);

GLFWmousebuttonfun nLastMouseButtonCallback;
GLFWkeyfun nLastKeyCallback;
GLFWcursorposfun nLastCursorPosCallback;
GLFWscrollfun nLastScrollCallback;

Vec2 nMousePosition;
Vec2 nMouseMotion;
Vec2 nMouseScroll;
bool nMouseFocus;
bool nKeyboardFocus;
Ds::Vector<int> nMousePressed;
Ds::Vector<int> nMouseReleased;
Ds::Vector<int> nKeyPressed;
Ds::Vector<int> nKeyReleased;

void Init()
{
  nMouseFocus = true;
  nKeyboardFocus = true;

  nLastMouseButtonCallback =
    glfwSetMouseButtonCallback(Viewport::Window(), MouseButtonCallback);
  nLastKeyCallback = glfwSetKeyCallback(Viewport::Window(), KeyCallback);
  nLastCursorPosCallback =
    glfwSetCursorPosCallback(Viewport::Window(), CursorPosCallback);
  nLastScrollCallback =
    glfwSetScrollCallback(Viewport::Window(), ScrollCallback);
}

void Update()
{
  nMouseMotion[0] = 0.0f;
  nMouseMotion[1] = 0.0f;
  nMouseScroll[0] = 0.0f;
  nMouseScroll[1] = 0.0f;
  nMousePressed.Clear();
  nMouseReleased.Clear();
  nKeyPressed.Clear();
  nKeyReleased.Clear();
  glfwPollEvents();
}

const Vec2& MousePosition()
{
  return nMousePosition;
}

// This will give the mouse position in the window. Both values in the returned
// vector are in the range of [-1, 1], where -1 represents the leftmost and
// bottommost edges of the window.
Vec2 StandardMousePosition()
{
  Vec2 standard = nMousePosition;
  standard[0] = 2.0f * (standard[0] / Viewport::Width() - 0.5f);
  standard[1] = -2.0f * (standard[1] / Viewport::Height() - 0.5f);
  return standard;
}

const Vec2& MouseMotion()
{
  return nMouseMotion;
}

const Vec2& MouseScroll()
{
  return nMouseScroll;
}

void SetMouseFocus(bool hasFocus)
{
  nMouseFocus = hasFocus;
}

void SetKeyboardFocus(bool hasFocus)
{
  nKeyboardFocus = hasFocus;
}

bool MousePressed(Mouse mouseButton)
{
  return nMouseFocus && nMousePressed.Contains((int)mouseButton);
}

bool MouseReleased(Mouse mouseButton)
{
  return nMouseFocus && nMouseReleased.Contains((int)mouseButton);
}

bool MouseDown(Mouse mouseButton)
{
  return nMouseFocus &&
    GLFW_PRESS == glfwGetMouseButton(Viewport::Window(), (int)mouseButton);
}

bool KeyPressed(Key key)
{
  return nKeyboardFocus && nKeyPressed.Contains((int)key);
}

bool KeyReleased(Key key)
{
  return nKeyboardFocus && nKeyReleased.Contains((int)key);
}

bool KeyDown(Key key)
{
  return nKeyboardFocus &&
    GLFW_PRESS == glfwGetKey(Viewport::Window(), (int)key);
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
  if (window != Viewport::Window()) {
    return;
  }

  if (action == GLFW_PRESS) {
    nMousePressed.Push(button);
  }
  else {
    nMouseReleased.Push(button);
  }
  if (nLastMouseButtonCallback != nullptr) {
    nLastMouseButtonCallback(window, button, action, mods);
  }
}

void KeyCallback(
  GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (window != Viewport::Window()) {
    return;
  }

  if (action == GLFW_PRESS) {
    nKeyPressed.Push(key);
  }
  else if (action == GLFW_RELEASE) {
    nKeyReleased.Push(key);
  }
  if (nLastKeyCallback != nullptr) {
    nLastKeyCallback(window, key, scancode, action, mods);
  }
}

void CursorPosCallback(GLFWwindow* window, double xPos, double yPos)
{
  Vec2 newPos;
  newPos[0] = (float)xPos;
  newPos[1] = (float)yPos;
  Vec2& oldPos = nMousePosition;
  nMouseMotion = newPos - oldPos;
  nMousePosition = newPos;
  if (nLastCursorPosCallback != nullptr) {
    nLastCursorPosCallback(window, xPos, yPos);
  }
}

void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
  if (nMouseFocus) {
    nMouseScroll[0] = (float)xOffset;
    nMouseScroll[1] = (float)yOffset;
    return;
  }
  nMouseScroll[0] = 0.0f;
  nMouseScroll[1] = 0.0f;
  if (nLastScrollCallback != nullptr) {
    nLastScrollCallback(window, xOffset, yOffset);
  }
}

} // namespace Input