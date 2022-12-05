#include <imgui/imgui.h>

#include "DirectionalLight.h"
#include "editor/Utility.h"

namespace Comp {

void DirectionalLight::VInit(const World::Object& owner)
{
  mDirection = smDefaultDirection;
  mAmbient = smDefaultAmbient;
  mDiffuse = smDefaultDiffuse;
  mSpecular = smDefaultSpecular;
}

void DirectionalLight::VSerialize(Vlk::Value& val)
{
  val("Direction") = mDirection;
  val("Ambient") = mAmbient;
  val("Diffuse") = mDiffuse;
  val("Specular") = mSpecular;
}

void DirectionalLight::VDeserialize(const Vlk::Explorer& ex)
{
  mDirection = ex("Direction").As<Vec3>(smDefaultDirection);
  mAmbient = ex("Ambient").As<Gfx::HdrColor>(smDefaultAmbient);
  mDiffuse = ex("Diffuse").As<Gfx::HdrColor>(smDefaultDiffuse);
  mSpecular = ex("Specular").As<Gfx::HdrColor>(smDefaultSpecular);
}

void DirectionalLight::VEdit(const World::Object& owner)
{
  float labelWidth = 65;
  ImGui::PushItemWidth(-labelWidth);
  ImGui::DragFloat3("Direction", mDirection.mD, 0.01f, -1.0f, 1.0f);
  ImGui::PopItemWidth();
  Editor::HdrColorEdit("Ambient", &mAmbient, -labelWidth);
  Editor::HdrColorEdit("Diffuse", &mDiffuse, -labelWidth);
  Editor::HdrColorEdit("Specular", &mSpecular, -labelWidth);
}

} // namespace Comp