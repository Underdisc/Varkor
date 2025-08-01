#include <imgui/imgui.h>

#include "comp/AlphaColor.h"

namespace Comp {

const Vec4 AlphaColor::smDefaultColor = {0.0f, 1.0f, 0.0f, 1.0f};

void AlphaColor::VInit(const World::Object& owner) {
  mColor = smDefaultColor;
}

void AlphaColor::VSerialize(Vlk::Value& val) {
  val("Color") = mColor;
}

void AlphaColor::VDeserialize(const Vlk::Explorer& ex) {
  mColor = ex("Color").As<Vec4>(smDefaultColor);
}

void AlphaColor::VEdit(const World::Object& owner) {
  ImGui::ColorEdit4("Color", mColor.mD);
}

} // namespace Comp
