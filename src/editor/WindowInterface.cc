#include "editor/WindowInterface.h"

namespace Editor {

WindowInterface::~WindowInterface() {}

void WindowInterface::HandleInterfaces()
{
  Interface::HandleInterfaces();
  Show();
}

} // namespace Editor
