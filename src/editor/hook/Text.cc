#include "editor/hook/Text.h"
#include "editor/Utility.h"
#include "gfx/Font.h"
#include "gfx/Shader.h"

namespace Editor {

bool Hook<Comp::Text>::Edit(const World::Object& object)
{
  Comp::Text* textComp = object.GetComponent<Comp::Text>();
  SelectAssetWidget<Gfx::Font>(&textComp->mFontId);
  SelectAssetWidget<Gfx::Shader>(&textComp->mShaderId);
  ImGui::PushItemWidth(-40.0f);
  ImGui::DragFloat("Width", &textComp->mWidth, 1.0f, 0.0f, FLT_MAX);
  const char* alignments[] = {"Left", "Center", "Right"};
  ImGui::Combo("Align", &(int)textComp->mAlign, alignments, 3);
  ImGui::ColorEdit3("Color", textComp->mColor.mD);
  ImGui::PopItemWidth();
  ImGui::PushID(0);
  InputTextMultiline("", {-1.0f, 100.0f}, &textComp->mText);
  ImGui::PopID();
  return false;
}

} // namespace Editor
