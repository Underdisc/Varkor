#include <imgui/imgui.h>

#include "comp/PointLight.h"
#include "comp/Transform.h"
#include "editor/Utility.h"
#include "editor/gizmos/Translator.h"
#include "vlk/Valkor.h"

namespace Comp {

const Gfx::HdrColor PointLight::smDefaultAmbient = {1.0f, 1.0f, 1.0f, 0.1f};
const Gfx::HdrColor PointLight::smDefaultDiffuse = {1.0f, 1.0f, 1.0f, 0.5f};
const Gfx::HdrColor PointLight::smDefaultSpecular = {1.0f, 1.0f, 1.0f, 1.0f};

void PointLight::VInit(const World::Object& owner) {
  mAmbient = smDefaultAmbient;
  mDiffuse = smDefaultDiffuse;
  mSpecular = smDefaultSpecular;
  mConstant = smDefaultConstant;
  mLinear = smDefaultLinear;
  mQuadratic = smDefaultQuadratic;
}

void PointLight::VSerialize(Vlk::Value& val) {
  val("Ambient") = mAmbient;
  val("Diffuse") = mDiffuse;
  val("Specular") = mSpecular;
  val("Constant") = mConstant;
  val("Linear") = mLinear;
  val("Quadratic") = mQuadratic;
}

void PointLight::VDeserialize(const Vlk::Explorer& ex) {
  mAmbient = ex("Ambient").As<Gfx::HdrColor>(smDefaultAmbient);
  mDiffuse = ex("Diffuse").As<Gfx::HdrColor>(smDefaultDiffuse);
  mSpecular = ex("Specular").As<Gfx::HdrColor>(smDefaultSpecular);
  mConstant = ex("Constant").As<float>(smDefaultConstant);
  mLinear = ex("Linear").As<float>(smDefaultLinear);
  mQuadratic = ex("Quadratic").As<float>(smDefaultQuadratic);
}

void PointLight::VRenderable(const World::Object& owner) {
  Gfx::Renderable::Icon icon;
  icon.mOwner = owner.mMemberId;
  auto& transform = owner.Get<Transform>();
  icon.mTranslation = transform.GetWorldTranslation(owner);
  icon.mColor = (Vec4)mDiffuse.TrueColor();
  icon.mColor[3] = 1.0f;
  icon.mMeshId = "vres/renderer:LightIcon";
  Gfx::Collection::Add(std::move(icon));
}

void PointLight::VEdit(const World::Object& owner) {
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
