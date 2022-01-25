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

std::string TypeData::AssessmentHeader() const
{
  std::stringstream error;
  error << mName << " component assessment: ";
  return error.str();
}

std::string TypeData::DifferenceHeader() const
{
  std::stringstream error;
  error << AssessmentHeader() << "Live and saved difference: ";
  return error.str();
}

void AssessSize(const TypeData& typeData, const Vlk::Explorer& compEx)
{
  Vlk::Explorer sizeEx = compEx("Size");
  if (!sizeEx.Valid())
  {
    std::stringstream error;
    error << typeData.AssessmentHeader() << "Missing Size field.";
    LogError(error.str().c_str());
    return;
  }

  size_t size = sizeEx.As<size_t>();
  if (size != typeData.mSize)
  {
    std::stringstream error;
    error << typeData.DifferenceHeader() << "(Live Size: " << typeData.mSize
          << "), (Saved Size: " << size << ")";
    LogError(error.str().c_str());
  }
}

void AssessDependencies(const TypeData& typeData, const Vlk::Explorer& compEx)
{
  Vlk::Explorer dependenciesEx = compEx("Dependencies");
  if (!dependenciesEx.Valid())
  {
    std::stringstream error;
    error << typeData.AssessmentHeader() << "Missing Dependencies field.";
    LogError(error.str().c_str());
    return;
  }

  Ds::Vector<TypeId> savedIds;
  for (int i = 0; i < dependenciesEx.Size(); ++i)
  {
    const std::string& dependencyName = dependenciesEx[i].As<std::string>();
    TypeId savedId = GetTypeId(dependencyName);
    savedIds.Push(savedId);
    if (!typeData.mDependencies.Contains(savedId))
    {
      std::stringstream error;
      error << typeData.DifferenceHeader() << "Live missing " << dependencyName
            << " dependency.";
      LogError(error.str().c_str());
    }
  }
  for (TypeId liveDependencyId : typeData.mDependencies)
  {
    if (!savedIds.Contains(liveDependencyId))
    {
      const TypeData& dependencyTypeData = GetTypeData(liveDependencyId);
      std::stringstream error;
      error << typeData.DifferenceHeader() << "Saved missing "
            << dependencyTypeData.mName << " dependency.";
      LogError(error.str().c_str());
    }
  }
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
    AssessSize(typeData, componentEx);
    AssessDependencies(typeData, componentEx);
  }
}

void SaveComponentsFile()
{
  Vlk::Value rootVal;
  rootVal("Version") = nVersion;
  Vlk::Value& componentsVal = rootVal("Components");
  for (TypeId id = 0; id < TypeDataCount(); ++id)
  {
    const TypeData& typeData = GetTypeData(id);
    Vlk::Value& typeDataVal = componentsVal(typeData.mName);
    typeDataVal("Size") = typeData.mSize;
    // Save the dependencies. We don't save dependants because they can be
    // inferred from the dependencies.
    Vlk::Value& dependenciesVal =
      typeDataVal("Dependencies")[{typeData.mDependencies.Size()}];
    for (size_t i = 0; i < typeData.mDependencies.Size(); ++i)
    {
      TypeId typeId = typeData.mDependencies[i];
      const TypeData& dependencyTypeData = GetTypeData(typeId);
      dependenciesVal[i] = dependencyTypeData.mName;
    }
  }
  Result result = rootVal.Write(nComponentsFilename);
  LogErrorIf(!result.Success(), result.mError.c_str());
}

} // namespace Comp
