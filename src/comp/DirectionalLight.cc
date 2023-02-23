#include <imgui/imgui.h>

#include "DirectionalLight.h"
#include "editor/Utility.h"

namespace Comp {

void DirectionalLight::VInit(const World::Object& owner)
{
  mTranslation = smDefaultTranslation;
  mDirection = smDefaultDirection;
  mAmbient = smDefaultAmbient;
  mDiffuse = smDefaultDiffuse;
  mSpecular = smDefaultSpecular;
}

void DirectionalLight::VSerialize(Vlk::Value& val)
{
  val("Translation") = mTranslation;
  val("Direction") = mDirection;
  val("Ambient") = mAmbient;
  val("Diffuse") = mDiffuse;
  val("Specular") = mSpecular;
}

void DirectionalLight::VDeserialize(const Vlk::Explorer& ex)
{
  mTranslation = ex("Translation").As<Vec3>(smDefaultTranslation);
  mDirection = ex("Direction").As<Vec3>(smDefaultTranslation);
  mAmbient = ex("Ambient").As<Gfx::HdrColor>(smDefaultAmbient);
  mDiffuse = ex("Diffuse").As<Gfx::HdrColor>(smDefaultDiffuse);
  mSpecular = ex("Specular").As<Gfx::HdrColor>(smDefaultSpecular);
}

void DirectionalLight::VRenderable(const World::Object& owner)
{
  Vec4 iconColor = (Vec4)mDiffuse.TrueColor();
  iconColor[3] = 1.0f;
  Gfx::Renderable::Collection::AddIcon(
    {owner.mMemberId, mTranslation, iconColor, "vres/renderer:Light"});
}

void DirectionalLight::VEdit(const World::Object& owner)
{
  float labelWidth = 65;
  ImGui::PushItemWidth(-labelWidth);
  ImGui::DragFloat3("Translation", mTranslation.mD, 0.01f);
  ImGui::DragFloat3("Direction", mDirection.mD, 0.01f, -1.0f, 1.0f);
  ImGui::PopItemWidth();
  Editor::HdrColorEdit("Ambient", &mAmbient, -labelWidth);
  Editor::HdrColorEdit("Diffuse", &mDiffuse, -labelWidth);
  Editor::HdrColorEdit("Specular", &mSpecular, -labelWidth);
}

} // namespace Comp