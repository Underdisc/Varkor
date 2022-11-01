#ifndef editor_Util_h
#define editor_Util_h

#include <imgui/imgui.h>
#include <string>

#include "editor/WindowInterface.h"
#include "gfx/HdrColor.h"

namespace Editor {

float CalcBufferWidth(const char* text);

// This acts as a replacement for ImGui::InputText when std::string types are
// being modified rather than c style character buffers.
bool InputText(const char* label, std::string* str, float itemWidth = -FLT_MIN);
bool InputTextMultiline(
  const char* label, const ImVec2& size, std::string* str);

void HelpMarker(const char* text);
void HdrColorEdit(const char* label, Gfx::HdrColor* hdrColor, float colorWidth);

} // namespace Editor

#endif
