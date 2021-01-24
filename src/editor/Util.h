#ifndef editor_Util_h
#define editor_Util_h

namespace Editor {

void ShowUtilWindows();
// This acts as a replacement for ImGui::InputText when std::string types are
// being modified rather than c style character buffers.
bool InputText(const char* label, std::string* str);
void StartFileSelection(void (*callback)(const std::string& file));

} // namespace Editor

#endif
