#ifndef editor_Util_h
#define editor_Util_h

#include <functional>

namespace Editor {

// This acts as a replacement for ImGui::InputText when std::string types are
// being modified rather than c style character buffers.
bool InputText(const char* label, std::string* str);
void OpenPopup(const std::string& name, const std::string& text);
void SelectFile(std::function<void(const std::string&)> callback);
void SaveFile(
  const std::string& defaultFilename,
  std::function<void(const std::string&)> callback);
void HelpMarker(const char* text);

// This function is only intended to be called by the main function driving the
// editor. Some of the editor utility functions create windows that will not
// close until a certain action is performed. This function is used to display
// those utility windows.
void ShowUtilWindows();

} // namespace Editor

#endif
