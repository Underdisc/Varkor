#include <sstream>

#include "Error.h"

namespace Editor {
namespace Hook {

template<typename T>
void Edit(T* component)
{
  std::string typeName(typeid(T).name());
  std::string::size_type loc = typeName.find_last_of(" ");
  typeName.erase(0, loc + 1);
  std::stringstream error;
  error << "There is no Edit<" << typeName << "> function.";
  LogAbort(error.str().c_str());
}

} // namespace Hook
} // namespace Editor
