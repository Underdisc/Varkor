#ifndef editor_LibraryInterface_h
#define editor_LibraryInterface_h

#include "editor/WindowInterface.h"
#include "rsl/Asset.h"

namespace Editor {

struct LibraryInterface: public WindowInterface
{
public:
  LibraryInterface();
  void Show();
  void ShowStatus(Rsl::Asset::Status status);
};

} // namespace Editor

#endif