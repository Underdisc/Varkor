#include <imgui/imgui.h>
#include <sstream>
#include <string>

#include "Error.h"
#include "editor/Primary.h"
#include "util/Utility.h"

namespace Editor {
namespace Hook {

// This acts like a unique pointer for gizmos. It will handle freeing the gizmo
// memory when its destructor is called.
struct GizmoStorage
{
  std::string mName;
  GizmoBase* mGizmo;

  GizmoStorage(const std::string& name, GizmoBase* gizmo):
    mName(name), mGizmo(gizmo)
  {}
  GizmoStorage(GizmoStorage&& other):
    mName(Util::Move(other.mName)), mGizmo(other.mGizmo)
  {}
  ~GizmoStorage()
  {
    delete mGizmo;
  }
  GizmoStorage& operator=(GizmoStorage&& other)
  {
    delete mGizmo;
    mName = Util::Move(other.mName);
    mGizmo = other.mGizmo;
    return *this;
  }
};

extern Ds::Vector<GizmoStorage> nGizmoStore;
constexpr int nInvalidGizmoIndex = -1;

template<typename T>
int FindGizmoIndex()
{
  std::string name(Util::GetLastName<T>());
  for (int i = 0; i < nGizmoStore.Size(); ++i)
  {
    if (nGizmoStore[i].mName == name)
    {
      return i;
    }
  }
  return nInvalidGizmoIndex;
}

template<typename T>
int GetExistingGizmoIndex()
{
  int gizmoIndex = FindGizmoIndex<T>();
  if (gizmoIndex == nInvalidGizmoIndex)
  {
    std::string fullName = Util::GetFullName<T>();
    std::stringstream error;
    error << "A gizmo for " << fullName << " has not been allocated.";
    LogAbort(error.str().c_str());
  }
  return gizmoIndex;
}

template<typename T>
bool ActiveGizmo()
{
  return FindGizmoIndex<T>() != nInvalidGizmoIndex;
}

template<typename T>
void StartGizmo()
{
  std::string name(Util::GetLastName<T>());
  Gizmo<T>* gizmo = alloc Gizmo<T>;
  nGizmoStore.Emplace(name, (GizmoBase*)gizmo);
}

template<typename T>
void EndGizmo()
{
  int gizmoIndex = GetExistingGizmoIndex<T>();
  nGizmoStore.LazyRemove(gizmoIndex);
}

template<typename T>
bool RunGizmo(T* component)
{
  int gizmoIndex = GetExistingGizmoIndex<T>();
  Gizmo<T>* gizmo = (Gizmo<T>*)nGizmoStore[gizmoIndex].mGizmo;
  return gizmo->Run(component);
}

template<typename T>
T* InspectComponent(const World::Object& object)
{
  T* component = object.GetComponent<T>();
  if (component == nullptr)
  {
    return nullptr;
  }
  std::string name(Util::GetLastName<T>());
  if (ImGui::CollapsingHeader(name.c_str()))
  {
    std::string removeButtonLabel = "Remove " + name;
    if (ImGui::Button(removeButtonLabel.c_str(), ImVec2(-1, 0)))
    {
      object.RemComponent<T>();
      return nullptr;
    }
    Edit<T>(component);
    return component;
  }
  return nullptr;
}

template<typename T>
T* InspectGizmoComponent(const World::Object& object)
{
  T* component = InspectComponent<T>(object);
  if (component != nullptr)
  {
    if (!ActiveGizmo<T>())
    {
      StartGizmo<T>();
    }
    bool editing = RunGizmo<T>(component);
    nSuppressObjectPicking |= editing;
  } else if (ActiveGizmo<T>())
  {
    EndGizmo<T>();
  }
  return component;
}

template<typename T>
void MakeComponentAvailable(const World::Object& object)
{
  if (object.HasComponent<T>())
  {
    return;
  }
  std::string name(Util::GetLastName<T>());
  if (ImGui::Button(name.c_str(), ImVec2(-1, 0)))
  {
    object.AddComponent<T>();
  }
}

template<typename T>
void Edit(T* component)
{
  std::string fullName = Util::GetFullName<T>();
  std::stringstream error;
  error << "There is no Edit<" << fullName << "> function defined.";
  LogAbort(error.str().c_str());
}

template<typename T>
Gizmo<T>::Gizmo()
{
  std::string fullName = Util::GetFullName<T>();
  std::stringstream error;
  error << "There is no Gizmo<" << fullName << "> structure defined.";
  LogAbort(error.str().c_str());
}

// This function should never be called. By defining it, we can avoid obtuse
// compile and link errors and instead abort when one uses the generic Gizmo
// structure instead of their own specialization.
template<typename T>
bool Gizmo<T>::Run(T* component)
{}

} // namespace Hook
} // namespace Editor
