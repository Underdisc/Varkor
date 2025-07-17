#include <imgui/imgui.h>

#include "DirectionalLight.h"
#include "comp/Transform.h"
#include "editor/Utility.h"

namespace Comp {

const Gfx::HdrColor DirectionalLight::smDefaultAmbient = {
  1.0f, 1.0f, 1.0f, 0.1f};
const Gfx::HdrColor DirectionalLight::smDefaultDiffuse = {
  1.0f, 1.0f, 1.0f, 0.5f};
const Gfx::HdrColor DirectionalLight::smDefaultSpecular = {
  1.0f, 1.0f, 1.0f, 1.0f};

void DirectionalLight::VInit(const World::Object& owner) {
  mAmbient = smDefaultAmbient;
  mDiffuse = smDefaultDiffuse;
  mSpecular = smDefaultSpecular;
}

void DirectionalLight::VSerialize(Vlk::Value& val) {
  val("Ambient") = mAmbient;
  val("Diffuse") = mDiffuse;
  val("Specular") = mSpecular;
}

void DirectionalLight::VDeserialize(const Vlk::Explorer& ex) {
  mAmbient = ex("Ambient").As<Gfx::HdrColor>(smDefaultAmbient);
  mDiffuse = ex("Diffuse").As<Gfx::HdrColor>(smDefaultDiffuse);
  mSpecular = ex("Specular").As<Gfx::HdrColor>(smDefaultSpecular);
}

void DirectionalLight::VRenderable(const World::Object& owner) {
  Gfx::Renderable::Icon icon;
  icon.mOwner = owner.mMemberId;
  auto& transform = owner.Get<Transform>();
  icon.mTranslation = transform.GetWorldTranslation(owner);
  icon.mColor = (Vec4)mDiffuse.TrueColor();
  icon.mColor[3] = 1.0f;
  icon.mMeshId = "vres/renderer:LightIcon";
  Gfx::Collection::Add(std::move(icon));
}

void DirectionalLight::VEdit(const World::Object& owner) {
  float labelWidth = 65;
  Editor::HdrColorEdit("Ambient", &mAmbient, -labelWidth);
  Editor::HdrColorEdit("Diffuse", &mDiffuse, -labelWidth);
  Editor::HdrColorEdit("Specular", &mSpecular, -labelWidth);
}

} // namespace Comp
