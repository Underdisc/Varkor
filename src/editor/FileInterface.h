#ifndef editor_UtilityInterfaces_h
#define editor_UtilityInterfaces_h

#include <functional>
#include <string>

#include "editor/Interface.h"

namespace Editor {

struct FileInterface: public Interface
{
public:
  enum class AccessType
  {
    Select,
    Save,
  };

public:
  FileInterface(
    std::function<void(const std::string&)> callback,
    AccessType accessType,
    const std::string& defaultFilename = "");
  void Show();

private:
  std::function<void(const std::string&)> mCallback;
  AccessType mAccessType;
  std::string mFile;
};

} // namespace Editor

#endif
