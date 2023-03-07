#ifndef editor_Util_h
#define editor_Util_h

#include <imgui/imgui.h>
#include <string>

#include "gfx/HdrColor.h"
#include "rsl/ResourceId.h"
#include "rsl/ResourceType.h"

namespace Editor {

float CalcBufferWidth(const char* text);

// This acts as a replacement for ImGui::InputText when std::string types are
// being modified rather than c style character buffers.
bool InputText(const char* label, std::string* str, float itemWidth = -FLT_MIN);
bool InputTextMultiline(
  const char* label, const ImVec2& size, std::string* str);

void DragResourceId(Rsl::ResTypeId resTypeId, const Rsl::ResId& resId);
void DropResourceIdWidget(
  Rsl::ResTypeId resTypeId,
  Rsl::ResId* resId,
  const char* prefix = "",
  float bufferWidth = 0.0f);

void DragResourceFile(const std::string& resFile);
void DropResourceFileWidget(const char* label, std::string* resFile);

void HelpMarker(const char* text);
void HdrColorEdit(const char* label, Gfx::HdrColor* hdrColor, float colorWidth);

} // namespace Editor

#endif
