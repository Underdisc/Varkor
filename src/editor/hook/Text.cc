#include "editor/hook/Text.h"
#include "editor/Utility.h"
#include "gfx/Font.h"
#include "gfx/Shader.h"

namespace Editor {

void Hook<Comp::Text>::Edit(const World::Object& object)
{
  Comp::Text& textComp = object.GetComponent<Comp::Text>();
  SelectAssetWidget<Gfx::Font>(&textComp.mFontId);
  SelectAssetWidget<Gfx::Shader>(&textComp.mShaderId);
  ImGui::PushItemWidth(-40.0f);
  ImGui::DragFloat("Width", &textComp.mWidth, 1.0f, 0.0f, FLT_MAX);
  const char* alignments[] = {"Left", "Center", "Right"};
  int alignment = (int)textComp.mAlign;
  ImGui::Combo("Align", &alignment, alignments, 3);
  textComp.mAlign = (Comp::Text::Alignment)alignment;
  ImGui::DragFloat("FillAmount", &textComp.mFillAmount, 0.01f, 0.0f, 1.0f);
  ImGui::PopItemWidth();
  ImGui::PushID(0);
  InputTextMultiline("", {-1.0f, 100.0f}, &textComp.mText);
  ImGui::PopID();
}

} // namespace Editor
