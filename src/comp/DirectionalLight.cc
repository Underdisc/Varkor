#include <imgui/imgui.h>

#include "DirectionalLight.h"
#include "comp/Transform.h"
#include "editor/Utility.h"

namespace Comp {

void DirectionalLight::VInit(const World::Object& owner)
{
  mAmbient = smDefaultAmbient;
  mDiffuse = smDefaultDiffuse;
  mSpecular = smDefaultSpecular;
}

void DirectionalLight::VSerialize(Vlk::Value& val)
{
  val("Ambient") = mAmbient;
  val("Diffuse") = mDiffuse;
  val("Specular") = mSpecular;
}

void DirectionalLight::VDeserialize(const Vlk::Explorer& ex)
{
  mAmbient = ex("Ambient").As<Gfx::HdrColor>(smDefaultAmbient);
  mDiffuse = ex("Diffuse").As<Gfx::HdrColor>(smDefaultDiffuse);
  mSpecular = ex("Specular").As<Gfx::HdrColor>(smDefaultSpecular);
}

void DirectionalLight::VRenderable(const World::Object& owner)
{
  auto& transform = owner.Get<Transform>();
  Vec3 translation = transform.GetWorldTranslation(owner);
  Vec4 iconColor = (Vec4)mDiffuse.TrueColor();
  iconColor[3] = 1.0f;
  Gfx::Renderable::Collection::AddIcon(
    {owner.mMemberId, translation, iconColor, "vres/renderer:LightIcon"});
}

void DirectionalLight::VEdit(const World::Object& owner)
{
  float labelWidth = 65;
  Editor::HdrColorEdit("Ambient", &mAmbient, -labelWidth);
  Editor::HdrColorEdit("Diffuse", &mDiffuse, -labelWidth);
  Editor::HdrColorEdit("Specular", &mSpecular, -labelWidth);
}

} // namespace Comp