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
// The mPath of the BrowseStatus returned will always contain the current path
// of the browser, but mFile will only be a non-empty string if a file is
// clicked on.
struct BrowseStatus
{
  std::string mPath;
  std::string mFile;
};
BrowseStatus BrowseWorkingDirectory(float remainingSpace)
{
  // Open the currently selected directory. We unroll the path to handle cases
  // where directories may have been deleted or renamed.
  static std::string path = ".";
  DIR* directory = opendir(path.c_str());
  while (directory == nullptr)
  {
    path.erase(path.find_last_of('/'));
    directory = opendir(path.c_str());
  }

  // This will prevent "." from showing up as an option and ".." when the
  // current path is ".".
  readdir(directory);
  if (path == ".")
  {
    readdir(directory);
  }

  // List all of the directories and files.
  ImGui::BeginChild("Entries", ImVec2(0, -remainingSpace), true);
  dirent* entry;
  BrowseStatus status;
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
    if (entry->d_name == std::string(".."))
    {
      // When going up a level, we remove a directory from the current path.
      path.erase(path.find_last_of('/'));
      continue;
    }
    if (isDir)
    {
      path.append("/");
      path.append(entry->d_name);
      continue;
    }
    status.mFile = entry->d_name;
  }
  closedir(directory);
  ImGui::EndChild();

  status.mPath = path;
  return status;
}

////////////////////////////////////////////////////////////////////////////////
std::function<void(const std::string&)> nSelectFileCallback = nullptr;
std::string nSelectFile = "";

void SelectFile(std::function<void(const std::string&)> callback)
{
  nSelectFileCallback = callback;
  nSelectFile = "";
}

void SelectFileWindow()
{
  if (nSelectFileCallback == nullptr)
  {
    return;
  }
  bool continueWindow = true;

  ImGui::Begin("Select File", &continueWindow);
  BrowseStatus status = BrowseWorkingDirectory(58);
  ImGui::Text("Directory: %s", status.mPath.c_str());
  if (!status.mFile.empty())
  {
    nSelectFile = status.mFile;
  }
  ImGui::Text("File: %s", nSelectFile.c_str());
  if (ImGui::Button("Select", ImVec2(-1, 0)))
  {
    nSelectFileCallback(status.mPath + "/" + nSelectFile);
    continueWindow = false;
  }
  ImGui::End();

  if (!continueWindow)
  {
    nSelectFileCallback = nullptr;
  }
}

////////////////////////////////////////////////////////////////////////////////
// todo: SaveFile is nearly a duplicate of SelectFile. This will be addressed in
// future commits when the editor code undergoes a major overhaul.
std::function<void(const std::string&)> nSaveFileCallback = nullptr;
std::string nSaveFile = "";

void SaveFile(
  const std::string& defaultFilename,
  std::function<void(const std::string&)> callback)
{
  nSaveFileCallback = callback;
  nSaveFile = defaultFilename;
}

void SaveFileWindow()
{
  if (nSaveFileCallback == nullptr)
  {
    return;
  }
  bool continueWindow = true;

  ImGui::Begin("Save File", &continueWindow);
  BrowseStatus status = BrowseWorkingDirectory(64);
  ImGui::Text("Directory: %s", status.mPath.c_str());
  if (!status.mFile.empty())
  {
    nSaveFile = status.mFile;
  }
  ImGui::PushItemWidth(-1);
  InputText("File", &nSaveFile);
  ImGui::PopItemWidth();
  if (ImGui::Button("Save", ImVec2(-1, 0)))
  {
    nSaveFileCallback(status.mPath + "/" + nSaveFile);
    continueWindow = false;
  }
  ImGui::End();

  if (!continueWindow)
  {
    nSaveFileCallback = nullptr;
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
  SelectFileWindow();
  SaveFileWindow();
}

} // namespace Editor
