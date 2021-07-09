#include <cstring>
#include <dirent/dirent.h>
#include <functional>
#include <imgui/imgui.h>
#include <string>

#include "Error.h"

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

////////////////////////////////////////////////////////////////////////////////
std::string nPopupName;
std::string nPopupText;
bool nPopupInitialized = false;

void OpenPopup(const std::string& name, const std::string& text)
{
  nPopupName = name;
  nPopupText = text;
  nPopupInitialized = false;
}

void PopupWindow()
{
  if (nPopupName.empty())
  {
    return;
  }
  if (!nPopupInitialized)
  {
    ImGui::OpenPopup(nPopupName.c_str());
    nPopupInitialized = true;
  }
  bool continuePopupWindow;
  bool popupOpen = ImGui::BeginPopupModal(
    nPopupName.c_str(),
    &continuePopupWindow,
    ImGuiWindowFlags_AlwaysAutoResize);
  if (popupOpen)
  {
    ImGui::Text(nPopupText.c_str());
    if (ImGui::Button("OK", ImVec2(-1, 0)))
    {
      continuePopupWindow = false;
    }
    ImGui::EndPopup();
  }
  if (!continuePopupWindow)
  {
    ImGui::CloseCurrentPopup();
    nPopupName = "";
    nPopupText = "";
  }
}

////////////////////////////////////////////////////////////////////////////////
std::function<void(const std::string&)> nFileSelectCallback = nullptr;
std::string nCurrentPath = ".";

void StartFileSelection(std::function<void(const std::string&)> callback)
{
  nFileSelectCallback = callback;
}

void FileSelectWindow()
{
  if (nFileSelectCallback == nullptr)
  {
    return;
  }
  bool continueFileSelectWindow = true;
  ImGui::Begin("Select File", &continueFileSelectWindow);
  ImGui::Text("Current Directory: %s", nCurrentPath.c_str());

  // Open the currently selected directory. We unroll the path to handle cases
  // where directories may have been deleted or renamed.
  DIR* directory = opendir(nCurrentPath.c_str());
  while (directory == nullptr)
  {
    nCurrentPath.erase(nCurrentPath.find_last_of('/'));
    directory = opendir(nCurrentPath.c_str());
  }

  // This will prevent "." from showing up as an option and ".." when the
  // current path is ".".
  readdir(directory);
  if (nCurrentPath.size() == 1)
  {
    readdir(directory);
  }

  // List all of the directories and files in the directory.
  ImGui::BeginChild("Files", ImVec2(0, 0), true);
  dirent* entry;
  while (entry = readdir(directory))
  {
    bool isDir = entry->d_type == DT_DIR;
    bool isFile = entry->d_type == DT_REG;
    if (!isDir && !isFile)
    {
      continue;
    }
    if (!ImGui::Selectable(entry->d_name, isDir))
    {
      continue;
    }

    // Reaching this point means that the entry was selected.
    if (isFile)
    {
      if (nCurrentPath.size() == 1)
      {
        nFileSelectCallback(entry->d_name);
      } else
      {
        nFileSelectCallback(nCurrentPath.substr(2) + "/" + entry->d_name);
      }
      nFileSelectCallback = nullptr;
      continue;
    }
    if (strcmp(entry->d_name, "..") == 0)
    {
      // When going up a level, we remove a directory from the current path.
      nCurrentPath.erase(nCurrentPath.find_last_of('/'));
      continue;
    }
    nCurrentPath += '/';
    nCurrentPath += entry->d_name;
  }
  closedir(directory);
  ImGui::EndChild();
  ImGui::End();

  if (!continueFileSelectWindow)
  {
    nFileSelectCallback = nullptr;
  }
}

////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
void ShowUtilWindows()
{
  PopupWindow();
  FileSelectWindow();
}

} // namespace Editor
