#include <imgui/imgui.h>
#include <string>

#include "Error.h"
#include "editor/Utility.h"

namespace Editor {

int InputTextCallback(ImGuiInputTextCallbackData* data)
{
  std::string* str = (std::string*)data->UserData;
  str->resize(data->BufTextLen);
  data->Buf = (char*)str->data();
  return 0;
}

bool InputText(const char* label, std::string* str)
{
  return ImGui::InputText(
    label,
    (char*)str->data(),
    str->capacity(),
    ImGuiInputTextFlags_CallbackResize,
    InputTextCallback,
    str);
}

bool InputTextMultiline(const char* label, const ImVec2& size, std::string* str)
{
  return ImGui::InputTextMultiline(
    label,
    (char*)str->data(),
    str->capacity(),
    size,
    ImGuiInputTextFlags_CallbackResize,
    InputTextCallback,
    str);
}

void HelpMarker(const char* text)
{
  ImGui::SameLine();
  ImGui::TextDisabled("(?)");
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();
    ImGui::TextUnformatted(text);
    ImGui::EndTooltip();
  }
}

void HdrColorEdit(const char* label, Gfx::HdrColor* hdrColor, float colorWidth)
{
  ImGui::PushID((void*)hdrColor);
  ImGui::PushItemWidth(60);
  ImGui::DragFloat("", &hdrColor->mIntensity, 0.01f, 0.0f, FLT_MAX, "I:%0.3f");
  ImGui::PopItemWidth();
  ImGui::PopID();
  ImGui::SameLine(0.0f, 4.0f);
  ImGui::PushItemWidth(colorWidth);
  ImGui::ColorEdit3(label, hdrColor->mColor.mD, ImGuiColorEditFlags_Float);
  ImGui::PopItemWidth();
}

} // namespace Editor
