#include <GLFW/glfw3.h>

#include <sstream>

#include "error.h"

#include "input.h"

GLFWwindow* Input::_window = nullptr;
Vec2 Input::_mouse_position = Vec2();
Vec2 Input::_mouse_motion = Vec2();
bool Input::_mouse_pressed[] = {};
bool Input::_mouse_released[] = {};
bool Input::_key_pressed[] = {};
bool Input::_key_released[] = {};

void Input::Init(GLFWwindow* window)
{
  _window = window;
  glfwSetMouseButtonCallback(window, MouseButtonCallack);
  glfwSetCursorPosCallback(window, CursorPosCallback);
}

void Input::Update()
{
  _mouse_motion[0] = 0.0f;
  _mouse_motion[1] = 0.0f;
  std::fill(_mouse_pressed, _mouse_pressed + (int)Mouse::Total, false);
  std::fill(_mouse_released, _mouse_released + (int)Mouse::Total, false);
  std::fill(_key_pressed, _key_pressed + (int)Key::Total, false);
  std::fill(_key_released, _key_released + (int)Key::Total, false);
  glfwPollEvents();
}

void Input::Purge() {}

Vec2 Input::MouseMotion()
{
  return _mouse_motion;
}

bool Input::MousePressed(Mouse mouse_button)
{
  return _mouse_pressed[(int)mouse_button];
}

bool Input::MouseReleased(Mouse mouse_button)
{
  return _mouse_released[(int)mouse_button];
}

bool Input::MouseDown(Mouse mouse_button)
{
  int glfw_button = MouseToGLFWMouse(mouse_button);
  int status = glfwGetMouseButton(_window, glfw_button);
  if (status == GLFW_PRESS)
  {
    return true;
  }
  return false;
}

void Input::MouseButtonCallack(
  GLFWwindow* window, int button, int action, int mods)
{
  if (window != _window)
  {
    return;
  }

  if (action == GLFW_PRESS)
  {
    _mouse_pressed[button] = true;
  } else
  {
    _mouse_released[button] = true;
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

Input::Mouse Input::GLFWMouseToMouse(int button)
{
  switch (button)
  {
  case GLFW_MOUSE_BUTTON_LEFT: return Mouse::Left;
  case GLFW_MOUSE_BUTTON_MIDDLE: return Mouse::Middle;
  case GLFW_MOUSE_BUTTON_RIGHT: return Mouse::Right;
  }
  return Mouse::Other;
}

int Input::MouseToGLFWMouse(Mouse button)
{
  switch (button)
  {
  case Mouse::Left: return GLFW_MOUSE_BUTTON_LEFT;
  case Mouse::Middle: return GLFW_MOUSE_BUTTON_MIDDLE;
  case Mouse::Right: return GLFW_MOUSE_BUTTON_RIGHT;
  }
  return _invalid_glfw_button;
}