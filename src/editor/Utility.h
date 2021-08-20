#ifndef editor_Util_h
#define editor_Util_h

#include <string>

#include "editor/WindowInterface.h"

namespace Editor {

struct PopupInterface: public WindowInterface
{
public:
  PopupInterface(const std::string& name, const std::string& text);
  void Show();

private:
  std::string mName;
  std::string mText;
};

// This acts as a replacement for ImGui::InputText when std::string types are
// being modified rather than c style character buffers.
bool InputText(const char* label, std::string* str);

void HelpMarker(const char* text);

} // namespace Editor

#endif
