#include <cstring>
#include <dirent/dirent.h>
#include <imgui/imgui.h>
#include <string>

#include "Error.h"

// todo: Reorder the implementations in this file to closely match the header.
// Additionally, put groups of functionality together. Don't keep global Editor
// variables at the top. Put them next to the code where they are used.
namespace Editor {

void (*nFileSelectCallback)(const std::string& file, void* data) = nullptr;
std::string nFileSelectPath;
void* nFileSelectData = nullptr;
bool nShowFileSelectWindow = false;

const char* nPopupName = nullptr;
std::string nPopupText;
bool nInitPopupWindow = false;
bool nShowPopupWindow = false;

void OpenPopup(const char* name, const std::string& text)
{
  nPopupName = name;
  nPopupText = text;
  nInitPopupWindow = true;
  nShowPopupWindow = true;
}

void PopupWindow()
{
  if (nInitPopupWindow)
  {
    ImGui::OpenPopup(nPopupName);
    nInitPopupWindow = false;
  }
  bool popupOpen = ImGui::BeginPopupModal(
    nPopupName, &nShowPopupWindow, ImGuiWindowFlags_AlwaysAutoResize);
  if (popupOpen)
  {
    ImGui::Text(nPopupText.c_str());
    if (ImGui::Button("OK", ImVec2(-1, 0)))
    {
      nShowPopupWindow = false;
    }
    ImGui::EndPopup();
  }
  if (!nShowPopupWindow)
  {
    ImGui::CloseCurrentPopup();
  }
}

void FileSelectWindow()
{
  LogAbortIf(
    nFileSelectCallback == nullptr,
    "The file selection callback must be set before calling this function.");

  ImGui::Begin("Select File", &nShowFileSelectWindow);
  ImGui::Text("Current Directory: %s", nFileSelectPath.c_str());

  // Open the currently selected directory.
  DIR* directory;
  if (nFileSelectPath.empty())
  {
    directory = opendir(".");
    // We iterate over a directory entry to avoid going up a level when we are
    // still in the executable's working directory.
    readdir(directory);
  } else
  {
    directory = opendir(nFileSelectPath.c_str());
  }
  // We iterate over the first entry since it will always be '.'.
  readdir(directory);

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
      nFileSelectPath += entry->d_name;
      std::string selectedFile(nFileSelectPath);
      nFileSelectCallback(selectedFile, nFileSelectData);
      nShowFileSelectWindow = false;
      continue;
    }

    // Whenever we go up a level, we remove a directory from the current path.
    if (strcmp(entry->d_name, "..") == 0)
    {
      std::string::size_type loc = nFileSelectPath.find_last_of('/');
      loc = nFileSelectPath.find_last_of('/', loc - 1);
      if (loc == std::string::npos)
      {
        nFileSelectPath.clear();
      } else
      {
        nFileSelectPath.erase(loc + 1);
      }
      continue;
    }
    nFileSelectPath += entry->d_name;
    nFileSelectPath += '/';
  }
  closedir(directory);
  ImGui::EndChild();
  ImGui::End();

  if (!nShowFileSelectWindow)
  {
    nFileSelectCallback = nullptr;
  }
}

void ShowUtilWindows()
{
  if (nShowFileSelectWindow)
  {
    FileSelectWindow();
  }
  if (nShowPopupWindow)
  {
    PopupWindow();
  }
}

void StartFileSelection(
  void (*callback)(const std::string& path, void* data), void* data)
{
  nShowFileSelectWindow = true;
  nFileSelectData = data;
  nFileSelectPath = "";
  nFileSelectCallback = callback;
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

} // namespace Editor
