#include <imgui/imgui.h>
#include <string>

#include "Error.h"
#include "editor/Utility.h"

namespace Editor {

float CalcBufferWidth(const char* text)
{
  return ImGui::CalcTextSize(text).x + 3.0f;
}

int InputTextCallback(ImGuiInputTextCallbackData* data)
{
  std::string* str = (std::string*)data->UserData;
  str->resize(data->BufTextLen);
  data->Buf = (char*)str->data();
  return 0;
}

bool InputText(const char* label, std::string* str, float itemWidth)
{
  ImGui::PushItemWidth(itemWidth);
  bool result = ImGui::InputText(
    label,
    (char*)str->data(),
    str->capacity(),
    ImGuiInputTextFlags_CallbackResize,
    InputTextCallback,
    str);
  ImGui::PopItemWidth();
  return result;
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

void DragResourceId(Rsl::ResourceTypeId resTypeId, const ResId& resId)
{
  const Rsl::ResTypeData& resTypeData = Rsl::GetResTypeData(resTypeId);
  std::string payloadName = resTypeData.mName;
  payloadName += "ResId";
  if (ImGui::BeginDragDropSource()) {
    ImGui::SetDragDropPayload(
      payloadName.c_str(), (void*)resId.mId.c_str(), resId.mId.size());
    ImGui::Text(resId.mId.c_str());
    ImGui::EndDragDropSource();
  }
}

void DropResourceIdWidget(
  Rsl::ResTypeId resTypeId, ResId* resId, const char* prefix, float bufferWidth)
{
  // Display an editable text box.
  const Rsl::ResTypeData& resTypeData = Rsl::GetResTypeData(resTypeId);
  std::string label = prefix;
  label += resTypeData.mName;
  label += "Id";
  if (bufferWidth == 0.0f) {
    bufferWidth = CalcBufferWidth(label.c_str());
  }
  ImGui::PushID((void*)resId);
  InputText(label.c_str(), &resId->mId, -bufferWidth);
  ImGui::PopID();

  // Make the text box a target for dropping ResourceIds.
  if (ImGui::BeginDragDropTarget()) {
    std::string payloadName = resTypeData.mName;
    payloadName += "ResId";
    const ImGuiPayload* payload =
      ImGui::AcceptDragDropPayload(payloadName.c_str());
    if (payload != nullptr) {
      std::string payloadString;
      payloadString.insert(0, (char*)payload->Data, payload->DataSize);
      *resId = payloadString;
    }
    ImGui::EndDragDropTarget();
  }
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
