#include <imgui/imgui.h>
#include <string>

#include "Error.h"
#include "editor/Utility.h"

namespace Editor {

PopupInterface::PopupInterface(
  const std::string& name, const std::string& text):
  mName(name), mText(text)
{}

void PopupInterface::Show()
{
  if (!ImGui::IsPopupOpen(mName.c_str()))
  {
    ImGui::OpenPopup(mName.c_str());
  }
  bool popupOpen = ImGui::BeginPopupModal(
    mName.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize);
  if (popupOpen)
  {
    ImGui::Text(mText.c_str());
    if (ImGui::Button("OK", ImVec2(-1, 0)))
    {
      mOpen = false;
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}

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

void HelpMarker(const char* text)
{
  ImGui::SameLine();
  ImGui::TextDisabled("(?)");
  if (ImGui::IsItemHovered())
  {
    ImGui::BeginTooltip();
    ImGui::TextUnformatted(text);
    ImGui::EndTooltip();
  }
}

} // namespace Editor
