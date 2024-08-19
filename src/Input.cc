#include "Input.h"
#include "Viewport.h"
#include "ds/Vector.h"
#include "ext/Tracy.h"
#include "math/Vector.h"
#include "vlk/Valkor.h"

namespace Input {

struct KeyStringMap
{
  Key mKey;
  const char* mString;
};

KeyStringMap nKeyStringMaps[] = {
  {Key::Unknown, "Unknown"},
  {Key::Space, "Space"},
  {Key::Apostrophe, "Apostrophe"},
  {Key::Comma, "Comma"},
  {Key::Minus, "Minus"},
  {Key::Period, "Period"},
  {Key::Slash, "Slash"},
  {Key::Zero, "Zero"},
  {Key::One, "One"},
  {Key::Two, "Two"},
  {Key::Three, "Three"},
  {Key::Four, "Four"},
  {Key::Five, "Five"},
  {Key::Six, "Six"},
  {Key::Seven, "Seven"},
  {Key::Eight, "Eight"},
  {Key::Nine, "Nine"},
  {Key::Semicolon, "Semicolon"},
  {Key::Equal, "Equal"},
  {Key::A, "A"},
  {Key::B, "B"},
  {Key::C, "C"},
  {Key::D, "D"},
  {Key::E, "E"},
  {Key::F, "F"},
  {Key::G, "G"},
  {Key::H, "H"},
  {Key::I, "I"},
  {Key::J, "J"},
  {Key::K, "K"},
  {Key::L, "L"},
  {Key::M, "M"},
  {Key::N, "N"},
  {Key::O, "O"},
  {Key::P, "P"},
  {Key::Q, "Q"},
  {Key::R, "R"},
  {Key::S, "S"},
  {Key::T, "T"},
  {Key::U, "U"},
  {Key::V, "V"},
  {Key::W, "W"},
  {Key::X, "X"},
  {Key::Y, "Y"},
  {Key::Z, "Z"},
  {Key::LeftBracket, "LeftBracket"},
  {Key::Backslash, "Backslash"},
  {Key::RightBracket, "RightBracket"},
  {Key::GraveAccent, "GraveAccent"},
  {Key::World1, "World1"},
  {Key::World2, "World2"},
  {Key::Escape, "Escape"},
  {Key::Enter, "Enter"},
  {Key::Tab, "Tab"},
  {Key::Backspace, "Backspace"},
  {Key::Insert, "Insert"},
  {Key::Del, "Del"},
  {Key::Right, "Right"},
  {Key::Left, "Left"},
  {Key::Down, "Down"},
  {Key::Up, "Up"},
  {Key::PageUp, "PageUp"},
  {Key::PageDown, "PageDown"},
  {Key::Home, "Home"},
  {Key::End, "End"},
  {Key::CapsLock, "CapsLock"},
  {Key::ScrollLock, "ScrollLock"},
  {Key::NumLock, "NumLock"},
  {Key::PrintScreen, "PrintScreen"},
  {Key::Pause, "Pause"},
  {Key::F1, "F1"},
  {Key::F2, "F2"},
  {Key::F3, "F3"},
  {Key::F4, "F4"},
  {Key::F5, "F5"},
  {Key::F6, "F6"},
  {Key::F7, "F7"},
  {Key::F8, "F8"},
  {Key::F9, "F9"},
  {Key::F10, "F10"},
  {Key::F11, "F11"},
  {Key::F12, "F12"},
  {Key::F13, "F13"},
  {Key::F14, "F14"},
  {Key::F15, "F15"},
  {Key::F16, "F16"},
  {Key::F17, "F17"},
  {Key::F18, "F18"},
  {Key::F19, "F19"},
  {Key::F20, "F20"},
  {Key::F21, "F21"},
  {Key::F22, "F22"},
  {Key::F23, "F23"},
  {Key::F24, "F24"},
  {Key::F25, "F25"},
  {Key::Kp0, "Kp0"},
  {Key::Kp1, "Kp1"},
  {Key::Kp2, "Kp2"},
  {Key::Kp3, "Kp3"},
  {Key::Kp4, "Kp4"},
  {Key::Kp5, "Kp5"},
  {Key::Kp6, "Kp6"},
  {Key::Kp7, "Kp7"},
  {Key::Kp8, "Kp8"},
  {Key::Kp9, "Kp9"},
  {Key::KpDecimal, "KpDecimal"},
  {Key::KpDivide, "KpDivide"},
  {Key::KpMultiply, "KpMultiply"},
  {Key::KpSubtract, "KpSubtract"},
  {Key::KpAdd, "KpAdd"},
  {Key::KpEnter, "KpEnter"},
  {Key::KpEqual, "KpEqual"},
  {Key::LeftShift, "LeftShift"},
  {Key::LeftControl, "LeftControl"},
  {Key::LeftAlt, "LeftAlt"},
  {Key::LeftSuper, "LeftSuper"},
  {Key::RightShift, "RightShift"},
  {Key::RightControl, "RightControl"},
  {Key::RightAlt, "RightAlt"},
  {Key::RightSuper, "RightSuper"},
  {Key::Menu, "Menu"},
};

const char* nActionStrings[] = {
  "MoveForward",
  "MoveBackward",
  "MoveLeft",
  "MoveRight",
  "MoveUp",
  "MoveDown",
  "ModeTranslate",
  "ModeScale",
  "ModeRotate",
  "ReferenceFrameWorld",
  "ReferenceFrameParent",
  "ReferenceFrameRelative",
  "ToggleEditor",
  "SaveLayer",
  "DuplicateEntity",
};

struct Keybind
{
  bool mControl;
  bool mAlt;
  Key mKey;
  bool ModifierTest() const;
};

bool Keybind::ModifierTest() const
{
  bool controlDown = KeyDown(Key::LeftControl) || KeyDown(Key::RightControl);
  if (mControl && !controlDown || (!mControl && controlDown)) {
    return false;
  }
  bool altDown = KeyDown(Key::LeftAlt) || KeyDown(Key::RightAlt);
  if (mAlt && !altDown || (!mAlt && altDown)) {
    return false;
  }
  return true;
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void KeyCallback(
  GLFWwindow* window, int key, int scancode, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xPos, double yPos);
void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
void InitKeybinds();
void SaveKeybinds();

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
const char* nKeybindsFile = "vKeybinds.vlk";
Keybind nActionKeybinds[(int)Action::Count];
Ds::Vector<Action> nActionsPressed;

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

  InitKeybinds();
}

void Update()
{
  ZoneScoped;

  nMouseMotion[0] = 0.0f;
  nMouseMotion[1] = 0.0f;
  nMouseScroll[0] = 0.0f;
  nMouseScroll[1] = 0.0f;
  nMousePressed.Clear();
  nMouseReleased.Clear();
  nKeyPressed.Clear();
  nKeyReleased.Clear();
  nActionsPressed.Clear();
  glfwPollEvents();

  // Update the status of actions.
  for (int action = 0; action <= (int)Action::Count; ++action) {
    const Keybind& keybind = nActionKeybinds[action];
    if (!keybind.ModifierTest()) {
      break;
    }
    if (KeyPressed(keybind.mKey)) {
      nActionsPressed.Push((Action)action);
    }
  }
}

const Vec2& MousePosition()
{
  return nMousePosition;
}

// This will give the mouse position in the window. Both values in the returned
// vector are in the range of [-1, 1], where -1 represents the leftmost and
// bottommost edges of the window.
Vec2 NdcMousePosition()
{
  Vec2 ndcPosition = nMousePosition;
  ndcPosition[0] = 2.0f * (ndcPosition[0] / Viewport::Width() - 0.5f);
  ndcPosition[1] = -2.0f * (ndcPosition[1] / Viewport::Height() - 0.5f);
  return ndcPosition;
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

void InitKeybinds()
{
  struct DefaultKeybind
  {
    Action mAction;
    Keybind mKeybind;
  };

  DefaultKeybind defaultKeybinds[(int)Action::Count] = {
    {Action::MoveForward, {false, false, Key::W}},
    {Action::MoveBackward, {false, false, Key::S}},
    {Action::MoveLeft, {false, false, Key::A}},
    {Action::MoveRight, {false, false, Key::D}},
    {Action::MoveUp, {false, false, Key::E}},
    {Action::MoveDown, {false, false, Key::Q}},
    {Action::ModeTranslate, {false, false, Key::Z}},
    {Action::ModeScale, {false, false, Key::X}},
    {Action::ModeRotate, {false, false, Key::C}},
    {Action::ReferenceFrameWorld, {true, false, Key::Z}},
    {Action::ReferenceFrameParent, {true, false, Key::X}},
    {Action::ReferenceFrameRelative, {true, false, Key::C}},
    {Action::ToggleEditor, {false, false, Key::GraveAccent}},
    {Action::SaveLayer, {true, false, Key::S}},
    {Action::DuplicateEntity, {true, false, Key::D}},
  };

  // Set all of the keybinds to the defaults.
  for (int i = 0; i < (int)Action::Count; ++i) {
    const DefaultKeybind& defaultKeybind = defaultKeybinds[i];
    Keybind& actionKeybind = nActionKeybinds[(int)defaultKeybind.mAction];
    actionKeybind = defaultKeybind.mKeybind;
  }

  // Read in the keybinds file and set custom keybinds.
  Vlk::Value rootVal;
  rootVal.Read(nKeybindsFile);
  Vlk::Explorer rootEx(rootVal);
  for (int i = 0; i < rootEx.Size(); ++i) {
    // Find the action keybind that's currently being set.
    Vlk::Explorer keybindEx = rootEx(i);
    Keybind* actionKeybind = nullptr;
    for (int j = 0; j < (int)Action::Count; ++j) {
      if (keybindEx.Key() == nActionStrings[j]) {
        actionKeybind = &nActionKeybinds[j];
      }
    }
    if (actionKeybind == nullptr) {
      std::stringstream error;
      error << "\"" << keybindEx.Key() << "\" is not a valid Action type.";
      LogError(error.str().c_str());
      break;
    }

    // Ensure the key value is a valid key string.
    std::string keyString = keybindEx("Key").As<std::string>();
    size_t keyStringMapsSize = sizeof(nKeyStringMaps) / sizeof(KeyStringMap);
    Key key = Key::Unknown;
    for (int j = 0; j < keyStringMapsSize; ++j) {
      if (nKeyStringMaps[j].mString == keyString) {
        key = nKeyStringMaps[j].mKey;
        break;
      }
    }
    if (key == Key::Unknown) {
      std::stringstream error;
      error << "\"" << keyString << "\" is not the name of a Key.";
      LogError(error.str().c_str());
      break;
    }

    // Set the keybind.
    actionKeybind->mControl = keybindEx("Control").As<bool>();
    actionKeybind->mAlt = keybindEx("Alt").As<bool>();
    actionKeybind->mKey = key;
  }
}

void SaveKeybinds()
{
  Vlk::Value rootVal;
  for (int i = 0; i < (int)Action::Count; ++i) {
    Vlk::Value& actionVal = rootVal(nActionStrings[i]);
    const Keybind& keybind = nActionKeybinds[i];
    actionVal("Control") = keybind.mControl;
    actionVal("Alt") = keybind.mAlt;
    size_t mapSize = sizeof(nKeyStringMaps) / sizeof(KeyStringMap);
    for (int i = 0; i < mapSize; ++i) {
      const KeyStringMap& keyStringMap = nKeyStringMaps[i];
      if (keyStringMap.mKey == keybind.mKey) {
        actionVal("Key") = keyStringMap.mString;
        break;
      }
    }
  }
  rootVal.Write(nKeybindsFile);
}

bool ActionActive(Action action)
{
  const Keybind& keybind = nActionKeybinds[(int)action];
  if (!keybind.ModifierTest()) {
    return false;
  }
  if (!KeyDown(keybind.mKey)) {
    return false;
  }
  return true;
}

bool ActionPressed(Action action)
{
  if (nActionsPressed.Contains(action)) {
    return true;
  }
  return false;
}

} // namespace Input