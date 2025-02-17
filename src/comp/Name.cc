#include "comp/Name.h"
#include "editor/Utility.h"

namespace Comp {

void Name::VInit(const World::Object& owner)
{
  mName = "Name";
}

void Name::VSerialize(Vlk::Value& val)
{
  val = mName;
}

void Name::VDeserialize(const Vlk::Explorer& ex)
{
  mName = ex.As<std::string>("Name");
}

void Name::VEdit(const World::Object& owner)
{
  Editor::InputText("Value", &mName);
}

} // namespace Comp