#ifndef editor_CoreInterface_h
#define editor_CoreInterface_h

#include "editor/Interface.h"

namespace Editor {

struct CoreInterface: public Interface {
public:
  void Init();
  void Show();

private:
  void FileMenu();
  void ViewMenu();
  template<typename T>
  void InterfaceMenuItem(const char* label);
  bool mShowImGuiDemo;
};

} // namespace Editor

#endif
