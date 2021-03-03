#include <imgui/imgui.h>

namespace Editor {

template<typename T>
std::string GetRawName()
{
  std::string fullName(typeid(T).name());
  std::string::size_type loc = fullName.find_last_of(" :");
  fullName.erase(0, loc + 1);
  return fullName;
}

template<typename T>
void InspectComponent(const Core::World::Object& object)
{
  T* comp = object.GetComponent<T>();
  if (comp == nullptr)
  {
    return;
  }
  std::string name(GetRawName<T>());
  if (ImGui::CollapsingHeader(name.c_str()))
  {
    comp->EditorHook();
  }
}

template<typename T>
void MakeComponentAvailable(const Core::World::Object& object)
{
  if (object.HasComponent<T>())
  {
    return;
  }
  std::string name(GetRawName<T>());
  if (ImGui::Button(name.c_str(), ImVec2(-1, 0)))
  {
    object.AddComponent<T>();
  }
}

} // namespace Editor
