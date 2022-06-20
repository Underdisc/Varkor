#include <dirent/dirent.h>
#include <imgui/imgui.h>

#include "AssetLibrary.h"
#include "editor/FileInterface.h"
#include "editor/Utility.h"

namespace Editor {

FileInterface::FileInterface(
  std::function<void(const std::string&)> callback,
  AccessType accessType,
  const std::string& defaultFilename):
  mCallback(callback), mAccessType(accessType), mFile(defaultFilename)
{}

void FileInterface::Show()
{
  // Get the full path based on the current path.
  static std::string path = ".";
  std::string fullPath;
  if (path.substr(0, 6) == "./vres") {
    fullPath = path;
  }
  else {
    fullPath = AssLib::PrependResDirectory(path);
  }

  // Begin the file select window. The remaining space is the amount of space
  // needed for elements at the bottom of the window.
  float remainingSpace;
  switch (mAccessType) {
  case AccessType::Select:
    remainingSpace = 58;
    ImGui::Begin("Select File", &mOpen);
    break;
  case AccessType::Save:
    remainingSpace = 64;
    ImGui::Begin("Save File", &mOpen);
    break;
  }
  ImGui::BeginChild("Entries", ImVec2(0, -remainingSpace), true);

  // Open the currently selected directory. We unroll the path to handle cases
  // where directories may have been deleted or renamed.
  DIR* directory = opendir(fullPath.c_str());
  while (directory == nullptr) {
    path.erase(path.find_last_of('/'));
    fullPath.erase(fullPath.find_last_of('/'));
    directory = opendir(fullPath.c_str());
  }

  // Remove "." from the selectables.
  readdir(directory);

  // Remove ".." from and add "vres" to the selectables.
  if (path == ".") {
    readdir(directory);
    if (ImGui::Selectable("vres", true)) {
      path = "./vres";
    }
  }

  // List all of the directories and files.
  dirent* entry;
  while (entry = readdir(directory)) {
    bool isDir = entry->d_type == DT_DIR;
    bool isFile = entry->d_type == DT_REG;
    if (!isDir && !isFile) {
      continue;
    }
    if (!ImGui::Selectable(entry->d_name, isDir)) {
      continue;
    }

    // Reaching this point means that the entry was selected.
    if (entry->d_name == std::string("..")) {
      // When going up a level, we remove a directory from the current path.
      path.erase(path.find_last_of('/'));
      continue;
    }
    if (isDir) {
      path.append("/");
      path.append(entry->d_name);
      continue;
    }
    mFile = entry->d_name;
  }
  closedir(directory);
  ImGui::EndChild();

  // Display the selected filename and path.
  ImGui::Text("Directory: %s", path.c_str());
  const char* buttonLabel = "";
  switch (mAccessType) {
  case AccessType::Select:
    ImGui::Text("File: %s", mFile.c_str());
    buttonLabel = "Select";
    break;
  case AccessType::Save:
    InputText("File", &mFile);
    buttonLabel = "Save";
    break;
  }
  if (ImGui::Button(buttonLabel, ImVec2(-1, 0))) {
    if (2 > path.size()) {
      mCallback(mFile);
    }
    else {
      mCallback(path.substr(2) + "/" + mFile);
    }
    mOpen = false;
  }
  ImGui::End();
}

} // namespace Editor
