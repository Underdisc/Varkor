#include <GLFW/glfw3.h>

#include "error.h"

#include "input.h"

GLFWwindow* Input::_window = nullptr;
Vec2 Input::_mouse_position = Vec2();
Vec2 Input::_mouse_motion = Vec2();
DS::Vector<int> Input::_mouse_pressed = DS::Vector<int>();
DS::Vector<int> Input::_mouse_released = DS::Vector<int>();
DS::Vector<int> Input::_key_pressed = DS::Vector<int>();
DS::Vector<int> Input::_key_released = DS::Vector<int>();

void Input::Init(GLFWwindow* window)
{
  _window = window;
  glfwSetMouseButtonCallback(window, MouseCallback);
  glfwSetKeyCallback(window, KeyCallback);
  glfwSetCursorPosCallback(window, CursorPosCallback);
}

void Input::Update()
{
  _mouse_motion[0] = 0.0f;
  _mouse_motion[1] = 0.0f;
  _mouse_pressed.Clear();
  _mouse_released.Clear();
  _key_pressed.Clear();
  _key_released.Clear();
  glfwPollEvents();
}

void Input::Purge() {}

Vec2 Input::MouseMotion()
{
  return _mouse_motion;
}

bool Input::MousePressed(Mouse mouse_button)
{
  return _mouse_pressed.Contains((int)mouse_button);
}

bool Input::MouseReleased(Mouse mouse_button)
{
  return _mouse_released.Contains((int)mouse_button);
}

bool Input::MouseDown(Mouse mouse_button)
{
  return GLFW_PRESS == glfwGetMouseButton(_window, (int)mouse_button);
}

bool Input::KeyPressed(Key key)
{
  return _key_pressed.Contains((int)key);
}

bool Input::KeyReleased(Key key)
{
  return _key_released.Contains((int)key);
}

bool Input::KeyDown(Key key)
{
  return glfwGetKey(_window, (int)key) == GLFW_PRESS;
}

void Input::MouseCallback(GLFWwindow* window, int button, int action, int mods)
{
  if (window != _window)
  {
    return;
  }

  if (action == GLFW_PRESS)
  {
    _mouse_pressed.Push(button);
  } else
  {
    _mouse_released.Push(button);
  }
}

void Input::KeyCallback(
  GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (window != _window)
  {
    return;
  }

  if (action == GLFW_PRESS)
  {
    _key_pressed.Push(key);
  } else if (action == GLFW_RELEASE)
  {
    _key_released.Push(key);
  }
}

void Input::CursorPosCallback(GLFWwindow* window, double x_pos, double y_pos)
{
  Vec2 new_pos;
  new_pos[0] = (float)x_pos;
  new_pos[1] = (float)y_pos;
  Vec2& old_pos = _mouse_position;
  _mouse_motion = new_pos - old_pos;
  _mouse_position = new_pos;
}