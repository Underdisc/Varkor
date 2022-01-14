#include "Type.h"
#include "vlk/Valkor.h"

namespace Comp {

int nVersion = -1;
const char* nComponentsFilename = "components.vlk";
Ds::Vector<TypeData> nTypeData;

int CreateId()
{
  static int id = 0;
  return id++;
}

const TypeData& GetTypeData(TypeId id)
{
  return nTypeData[id];
}

size_t TypeDataCount()
{
  return nTypeData.Size();
}

TypeId GetTypeId(const std::string& typeName)
{
  for (TypeId typeId = 0; typeId < nTypeData.Size(); ++typeId)
  {
    if (typeName == GetTypeData(typeId).mName)
    {
      return typeId;
    }
  }
  std::string error;
  error += typeName + " does not have a TypeId.";
  LogAbort(error.c_str());
  return nInvalidTypeId;
}

void AssessComponentsFile()
{
  // Ensure that the components file contains the required information and
  // initialize the version.
  Vlk::Value rootVal;
  Result result = rootVal.Read(nComponentsFilename);
  if (!result.Success())
  {
    LogError(result.mError.c_str());
    return;
  }
  Vlk::Explorer rootEx(rootVal);
  Vlk::Explorer versionEx = rootEx("Version");
  Vlk::Explorer componentsEx = rootEx("Components");
  if (!versionEx.Valid() || !componentsEx.Valid())
  {
    std::stringstream error;
    error << "The root Value must contain a :Version: and a :Components: Pair.";
    LogAbort(error.str().c_str());
  }
  nVersion = versionEx.As<int>();

  // Display errors that show missing information or any changes to components.
  for (TypeId id = 0; id < TypeDataCount(); ++id)
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
    size_t size = sizeEx.As<size_t>();
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

void SaveComponentsFile()
{
  Vlk::Value rootVal;
  rootVal("Version") = nVersion + 1;
  Vlk::Value& componentsVal = rootVal("Components");
  for (TypeId id = 0; id < TypeDataCount(); ++id)
  {
    const TypeData& typeData = GetTypeData(id);
    Vlk::Value& typeDataVal = componentsVal(typeData.mName);
    typeDataVal("Size") = typeData.mSize;
  }
  Result result = rootVal.Write(nComponentsFilename);
  LogErrorIf(!result.Success(), result.mError.c_str());
}

} // namespace Comp
