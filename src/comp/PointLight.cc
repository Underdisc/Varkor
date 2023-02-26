#include <imgui/imgui.h>

#include "comp/PointLight.h"
#include "comp/Transform.h"
#include "editor/Utility.h"
#include "editor/gizmos/Translator.h"
#include "vlk/Valkor.h"

namespace Comp {

void PointLight::VInit(const World::Object& owner)
{
  mAmbient = smDefaultAmbient;
  mDiffuse = smDefaultDiffuse;
  mSpecular = smDefaultSpecular;
  mConstant = smDefaultConstant;
  mLinear = smDefaultLinear;
  mQuadratic = smDefaultQuadratic;
}

void PointLight::VSerialize(Vlk::Value& val)
{
  val("Ambient") = mAmbient;
  val("Diffuse") = mDiffuse;
  val("Specular") = mSpecular;
  val("Constant") = mConstant;
  val("Linear") = mLinear;
  val("Quadratic") = mQuadratic;
}

void PointLight::VDeserialize(const Vlk::Explorer& ex)
{
  mAmbient = ex("Ambient").As<Gfx::HdrColor>(smDefaultAmbient);
  mDiffuse = ex("Diffuse").As<Gfx::HdrColor>(smDefaultDiffuse);
  mSpecular = ex("Specular").As<Gfx::HdrColor>(smDefaultSpecular);
  mConstant = ex("Constant").As<float>(smDefaultConstant);
  mLinear = ex("Linear").As<float>(smDefaultLinear);
  mQuadratic = ex("Quadratic").As<float>(smDefaultQuadratic);
}

void PointLight::VRenderable(const World::Object& owner)
{
  auto& transform = owner.Get<Transform>();
  Vec3 translation = transform.GetWorldTranslation(owner);
  Vec4 iconColor = (Vec4)mDiffuse.TrueColor();
  iconColor[3] = 1.0f;
  Gfx::Renderable::Collection::AddIcon(
    {owner.mMemberId, translation, iconColor, "vres/renderer:Light"});
}

void PointLight::VEdit(const World::Object& owner)
{
  float labelWidth = 65;
  ImGui::PushItemWidth(-labelWidth);
  ImGui::DragFloat("Constant", &mConstant, 0.01f, 1.0f, 2.0f);
  ImGui::DragFloat("Linear", &mLinear, 0.01f, 0.0f, 2.0f);
  ImGui::DragFloat("Quadratic", &mQuadratic, 0.01f, 0.0f, 2.0f);
  ImGui::PopItemWidth();
  Editor::HdrColorEdit("Ambient", &mAmbient, -labelWidth);
  Editor::HdrColorEdit("Diffuse", &mDiffuse, -labelWidth);
  Editor::HdrColorEdit("Specular", &mSpecular, -labelWidth);
}

} // namespace Comp
