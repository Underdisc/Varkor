#include "comp/AlphaColor.h"

namespace Comp {

void AlphaColor::VInit(const World::Object& owner)
{
  mColor = smDefaultColor;
}

void AlphaColor::VSerialize(Vlk::Value& val)
{
  val("Color") = mColor;
}

void AlphaColor::VDeserialize(const Vlk::Explorer& ex)
{
  mColor = ex("Color").As<Vec4>(smDefaultColor);
}

} // namespace Comp

namespace Editor {

void Hook<Comp::AlphaColor>::Edit(const World::Object& object)
{
  auto& colorComp = object.Get<Comp::AlphaColor>();
  ImGui::ColorEdit4("Color", colorComp.mColor.mD);
}

} // namespace Editor
