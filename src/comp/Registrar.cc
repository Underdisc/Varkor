#include <sstream>

#include "comp/Model.h"
#include "comp/Transform.h"
#include "comp/Type.h"
#include "vlk/Explorer.h"
#include "vlk/Pair.h"

namespace Comp {

void RegisterTypes()
{
  Type<Model>::Register();
  Type<Transform>::Register();
}

void RegisterHooks()
{
  Type<Model>::RegisterEditHook();
  Type<Transform>::RegisterEditHook();
  Type<Transform>::RegisterGizmo();
}

int nVersion;
const char* nComponentsFilename = "components.vlk";
void AssessComponentsFile()
{
  // Ensure that the components file contains the required information and
  // initialize the version.
  Vlk::Pair rootVlk;
  Util::Result result = rootVlk.Read(nComponentsFilename);
  if (!result.Success())
  {
    LogError(result.mError.c_str());
    return;
  }
  Vlk::Explorer rootEx(rootVlk);
  Vlk::Explorer versionEx = rootEx("Version");
  Vlk::Explorer componentsEx = rootEx("Components");
  if (!versionEx.Valid() || !componentsEx.Valid())
  {
    std::stringstream error;
    error << "The root Pair must contain a :Version: and a :Components: Pair.";
    LogAbort(error.str().c_str());
  }
  nVersion = versionEx.As<int>();

  // Display errors that show missing information or any changes to components.
  for (TypeId id = 0; id < nTypeData.Size(); ++id)
  {
    const TypeData& typeData = GetTypeData(id);
    Vlk::Explorer componentEx = componentsEx(id);
    if (!componentEx.Valid())
    {
      std::stringstream error;
      error << "The \"" << typeData.mName
            << "\" component had no entry saved at Id (" << id << ").";
      LogError(error.str().c_str());
      continue;
    }
    if (componentEx.Key() != typeData.mName)
    {
      std::stringstream error;
      error << "The \"" << typeData.mName
            << "\" component's name differed from the saved name \""
            << componentEx.Key() << "\" at Id (" << id << ").";
      LogError(error.str().c_str());
      continue;
    }
    Vlk::Explorer sizeEx = componentEx("Size");
    if (!sizeEx.Valid())
    {
      std::stringstream error;
      error << "The \"" << typeData.mName << "\" component had no Size field.";
      LogError(error.str().c_str());
      continue;
    }
    int size = sizeEx.As<int>();
    if (size != typeData.mSize)
    {
      std::stringstream error;
      error << "The \"" << typeData.mName << "\" component's true size ("
            << typeData.mSize << ") differed from the saved size (" << size
            << ").";
      LogError(error.str().c_str());
    }
  }
}

void SaveComponents()
{
  Vlk::Pair rootVlk;
  rootVlk("Version") = nVersion + 1;
  Vlk::Pair& componentsVlk = rootVlk("Components");
  for (TypeId id = 0; id < nTypeData.Size(); ++id)
  {
    const TypeData& typeData = GetTypeData(id);
    Vlk::Pair& typeDataVlk = componentsVlk(typeData.mName);
    typeDataVlk("Size") = typeData.mSize;
  }
  Util::Result result = rootVlk.Write(nComponentsFilename);
  LogErrorIf(!result.Success(), result.mError.c_str());
}

void Init()
{
  RegisterTypes();
  RegisterHooks();
  AssessComponentsFile();
}

} // namespace Comp
