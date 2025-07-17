#include <cmath>
#include <imgui/imgui.h>

#include "comp/SpotLight.h"
#include "comp/Transform.h"
#include "editor/Utility.h"
#include "editor/gizmos/Translator.h"
#include "math/Constants.h"
#include "vlk/Valkor.h"

namespace Comp {

const Gfx::HdrColor SpotLight::smDefaultAmbient = {1.0f, 1.0f, 1.0f, 0.1f};
const Gfx::HdrColor SpotLight::smDefaultDiffuse = {1.0f, 1.0f, 1.0f, 0.5f};
const Gfx::HdrColor SpotLight::smDefaultSpecular = {1.0f, 1.0f, 1.0f, 1.0f};

void SpotLight::VInit(const World::Object& owner) {
  mAmbient = smDefaultAmbient;
  mDiffuse = smDefaultDiffuse;
  mSpecular = smDefaultSpecular;
  mConstant = smDefaultConstant;
  mLinear = smDefaultLinear;
  mQuadratic = smDefaultQuadratic;
  SetInnerCutoff(smDefaultInnerCutoff);
  SetOuterCutoff(smDefaultOuterCutoff);
}

void SpotLight::VSerialize(Vlk::Value& val) {
  val("Ambient") = mAmbient;
  val("Diffuse") = mDiffuse;
  val("Specular") = mSpecular;
  val("Constant") = mConstant;
  val("Linear") = mLinear;
  val("Quadratic") = mQuadratic;
  val("InnerCutoff") = GetInnerCutoff();
  val("OuterCutoff") = GetOuterCutoff();
}

void SpotLight::VDeserialize(const Vlk::Explorer& ex) {
  mAmbient = ex("Ambient").As<Gfx::HdrColor>(smDefaultAmbient);
  mDiffuse = ex("Diffuse").As<Gfx::HdrColor>(smDefaultDiffuse);
  mSpecular = ex("Specular").As<Gfx::HdrColor>(smDefaultSpecular);
  mConstant = ex("Constant").As<float>(smDefaultConstant);
  mLinear = ex("Linear").As<float>(smDefaultLinear);
  mQuadratic = ex("Quadratic").As<float>(smDefaultQuadratic);
  SetInnerCutoff(ex("InnerCutoff").As<float>(smDefaultInnerCutoff));
  SetOuterCutoff(ex("OuterCutoff").As<float>(smDefaultOuterCutoff));
}

void SpotLight::VRenderable(const World::Object& owner) {
  Gfx::Renderable::Icon icon;
  icon.mOwner = owner.mMemberId;
  auto& transform = owner.Get<Transform>();
  icon.mTranslation = transform.GetWorldTranslation(owner);
  icon.mColor = (Vec4)mDiffuse.TrueColor();
  icon.mColor[3] = 1.0f;
  icon.mMeshId = "vres/renderer:LightIcon";
  Gfx::Collection::Add(std::move(icon));
}

void SpotLight::SetInnerCutoff(float angle) {
  mInnerCutoff = cosf(angle);
}

void SpotLight::SetOuterCutoff(float angle) {
  mOuterCutoff = cosf(angle);
}

float SpotLight::GetInnerCutoff() const {
  return acosf(mInnerCutoff);
}

float SpotLight::GetOuterCutoff() const {
  return acosf(mOuterCutoff);
}

void SpotLight::VEdit(const World::Object& owner) {
  float labelWidth = 65;
  ImGui::PushItemWidth(-labelWidth);
  ImGui::DragFloat("Constant", &mConstant, 0.01f, 1.0f, 2.0f);
  ImGui::DragFloat("Linear", &mLinear, 0.01f, 0.0f, 2.0f);
  ImGui::DragFloat("Quadratic", &mQuadratic, 0.01f, 0.0f, 2.0f);
  ImGui::PopItemWidth();
  Editor::HdrColorEdit("Ambient", &mAmbient, -labelWidth);
  Editor::HdrColorEdit("Diffuse", &mDiffuse, -labelWidth);
  Editor::HdrColorEdit("Specular", &mSpecular, -labelWidth);

  ImGui::PushItemWidth(-labelWidth);
  float innerCutoff = GetInnerCutoff();
  float outerCutoff = GetOuterCutoff();
  ImGui::DragFloat("Inner Cutoff", &innerCutoff, 0.01f, 0.0f, outerCutoff);
  ImGui::DragFloat(
    "Outer Cutoff", &outerCutoff, 0.01f, innerCutoff, Math::nPiO2);
  SetInnerCutoff(innerCutoff);
  SetOuterCutoff(outerCutoff);
  ImGui::PopItemWidth();
}

} // namespace Comp
