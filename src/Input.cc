#include <GLFW/glfw3.h>

#include "ds/vector.hh"
#include "math/vector.hh"
#include "viewport.h"

#include "input.h"

namespace Input {

void MouseCallback(GLFWwindow* window, int button, int action, int mods);
void KeyCallback(
  GLFWwindow* window, int key, int scancode, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xPos, double yPos);

Vec2 nMousePosition;
Vec2 nMouseMotion;
DS::Vector<int> nMousePressed;
DS::Vector<int> nMouseReleased;
DS::Vector<int> nKeyPressed;
DS::Vector<int> nKeyReleased;

void Input::Init()
{
  glfwSetMouseButtonCallback(Viewport::Window(), MouseCallback);
  glfwSetKeyCallback(Viewport::Window(), KeyCallback);
  glfwSetCursorPosCallback(Viewport::Window(), CursorPosCallback);
}

void Input::Update()
{
  nMouseMotion[0] = 0.0f;
  nMouseMotion[1] = 0.0f;
  nMousePressed.Clear();
  nMouseReleased.Clear();
  nKeyPressed.Clear();
  nKeyReleased.Clear();
  glfwPollEvents();
}

Vec2 Input::MouseMotion()
{
  return nMouseMotion;
}

bool Input::MousePressed(Mouse mouseButton)
{
  return nMousePressed.Contains((int)mouseButton);
}

bool Input::MouseReleased(Mouse mouseButton)
{
  return nMouseReleased.Contains((int)mouseButton);
}

bool Input::MouseDown(Mouse mouseButton)
{
  return GLFW_PRESS == glfwGetMouseButton(Viewport::Window(), (int)mouseButton);
}

bool Input::KeyPressed(Key key)
{
  return nKeyPressed.Contains((int)key);
}

bool Input::KeyReleased(Key key)
{
  return nKeyReleased.Contains((int)key);
}

bool Input::KeyDown(Key key)
{
  return glfwGetKey(Viewport::Window(), (int)key) == GLFW_PRESS;
}

void Input::MouseCallback(GLFWwindow* window, int button, int action, int mods)
{
  if (window != Viewport::Window())
  {
    return;
  }

  if (action == GLFW_PRESS)
  {
    nMousePressed.Push(button);
  } else
  {
    nMouseReleased.Push(button);
  }
}

void Input::KeyCallback(
  GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (window != Viewport::Window())
  {
    return;
  }

  if (action == GLFW_PRESS)
  {
    nKeyPressed.Push(key);
  } else if (action == GLFW_RELEASE)
  {
    nKeyReleased.Push(key);
  }
}

void Input::CursorPosCallback(GLFWwindow* window, double xPos, double yPos)
{
  Vec2 newPos;
  newPos[0] = (float)xPos;
  newPos[1] = (float)yPos;
  Vec2& oldPos = nMousePosition;
  nMouseMotion = newPos - oldPos;
  nMousePosition = newPos;
}

} // namespace Input