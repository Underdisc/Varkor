#include <string>

#include "editor/Editor.h"
#include "editor/Interface.h"
#include "editor/WindowInterface.h"

namespace Editor {

Interface::Interface(): mOpen(true) {}

Interface::~Interface() {}

void Interface::HandleInterfaces()
{
  // Remove all of the closed interfaces. This happens before inserting the
  // StagedInterfaces to account for the same Interface being closed and opened
  // during the same frame.
  Ds::Vector<std::string> closedInterfaces;
  auto it = mInterfaces.begin();
  auto itE = mInterfaces.end();
  while (it != itE) {
    if (!it->mValue->mOpen) {
      it->mValue->PurgeInterfaces();
      closedInterfaces.Push(it->Key());
    }
    ++it;
  }
  for (const std::string& closed : closedInterfaces) {
    Interface* interface = mInterfaces.Get(closed);
    delete interface;
    mInterfaces.Remove(closed);
  }

  // Insert the staged interfaces that are still open.
  for (const StagedInterface& staged : mStagedInterfaces) {
    if (staged.mInterface->mOpen) {
      mInterfaces.Insert(staged.mName, staged.mInterface);
    }
    else {
      staged.mInterface->PurgeInterfaces();
      delete staged.mInterface;
    }
  }
  mStagedInterfaces.Clear();

  // Handle all of the interfaces.
  it = mInterfaces.begin();
  itE = mInterfaces.end();
  while (it != itE) {
    it->mValue->HandleInterfaces();
    ++it;
  }
}

void Interface::PurgeInterfaces()
{
  auto it = mInterfaces.begin();
  auto itE = mInterfaces.end();
  while (it != itE) {
    it->mValue->PurgeInterfaces();
    delete it->mValue;
    ++it;
  }
  mInterfaces.Clear();
}

} // namespace Editor
