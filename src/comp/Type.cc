#include "Type.h"
#include "rsl/Library.h"
#include "vlk/Valkor.h"

namespace Comp {

int nVersion = -1;
Ds::Vector<TypeData> nTypeData;

int CreateId() {
  static int id = 0;
  return id++;
}

const TypeData& GetTypeData(TypeId id) {
  return nTypeData[id];
}

size_t TypeDataCount() {
  return nTypeData.Size();
}

TypeId GetTypeId(const std::string& typeName) {
  for (TypeId typeId = 0; typeId < nTypeData.Size(); ++typeId) {
    if (typeName == GetTypeData(typeId).mName) {
      return typeId;
    }
  }
  return nInvalidTypeId;
}

std::string AssessmentHeader(TypeId id) {
  const TypeData& typeData = GetTypeData(id);
  std::stringstream header;
  header << "Component " << id << " (" << typeData.mName << ") assessment: ";
  return header.str();
}

std::string DifferenceHeader(TypeId id) {
  std::stringstream header;
  header << AssessmentHeader(id) << "Live versus saved difference: ";
  return header.str();
}

void AssessName(TypeId id, const Vlk::Explorer& compEx) {
  Vlk::Explorer nameEx = compEx("Name");
  if (!nameEx.Valid()) {
    std::stringstream error;
    error << AssessmentHeader(id) << "Missing Name field.";
    LogError(error.str().c_str());
    return;
  }

  const TypeData& typeData = GetTypeData(id);
  std::string name = nameEx.As<std::string>();
  if (typeData.mName != name) {
    std::stringstream error;
    error << DifferenceHeader(id) << "Live Name: " << typeData.mName
          << ", Saved Name: " << name;
    LogError(error.str().c_str());
  }
}

void AssessSize(TypeId id, const Vlk::Explorer& compEx) {
  Vlk::Explorer sizeEx = compEx("Size");
  if (!sizeEx.Valid()) {
    std::stringstream error;
    error << AssessmentHeader(id) << "Missing Size field.";
    LogError(error.str().c_str());
    return;
  }

  const TypeData& typeData = GetTypeData(id);
  size_t size = sizeEx.As<size_t>();
  if (typeData.mSize != size) {
    std::stringstream error;
    error << DifferenceHeader(id) << "Live Size: " << typeData.mSize
          << ", Saved Size: " << size;
    LogError(error.str().c_str());
  }
}

void AssessDependencies(TypeId id, const Vlk::Explorer& compEx) {
  Vlk::Explorer dependenciesEx = compEx("Dependencies");
  if (!dependenciesEx.Valid()) {
    std::stringstream error;
    error << AssessmentHeader(id) << "Missing Dependencies field.";
    LogError(error.str().c_str());
    return;
  }

  const TypeData& typeData = GetTypeData(id);
  // Find saved dependencies that do not exist on the live component.
  Ds::Vector<TypeId> savedIds;
  for (int i = 0; i < dependenciesEx.Size(); ++i) {
    const std::string& dependencyName = dependenciesEx[i].As<std::string>();
    TypeId savedId = GetTypeId(dependencyName);
    if (savedId == nInvalidTypeId) {
      std::stringstream error;
      error << AssessmentHeader(id) << "Saved " << dependencyName
            << " dependency is not an existing component's name.";
      LogError(error.str().c_str());
      continue;
    }
    savedIds.Push(savedId);
    if (!typeData.mDependencies.Contains(savedId)) {
      std::stringstream error;
      error << DifferenceHeader(id) << "Live missing " << savedId << " ("
            << dependencyName << ") dependency.";
      LogError(error.str().c_str());
    }
  }

  // Find live dependencies that do not exist of the saved component.
  for (TypeId liveId: typeData.mDependencies) {
    if (!savedIds.Contains(liveId)) {
      const TypeData& dependencyTypeData = GetTypeData(liveId);
      std::stringstream error;
      error << DifferenceHeader(id) << "Saved missing " << liveId << " ("
            << dependencyTypeData.mName << ") dependency.";
      LogError(error.str().c_str());
    }
  }
}

const char* nComponentsFilename = "components.vlk";
void AssessComponentsFile() {
  if (Rsl::IsStandalone()) {
    return;
  }

  // Ensure that the components file contains the required information and
  // initialize the version.
  Vlk::Value rootVal;
  std::string componentsFile = Rsl::PrependResDirectory(nComponentsFilename);
  Result result = rootVal.Read(componentsFile.c_str());
  if (!result.Success()) {
    LogError(result.mError.c_str());
    return;
  }
  Vlk::Explorer rootEx(rootVal);
  Vlk::Explorer versionEx = rootEx("Version");
  Vlk::Explorer componentsEx = rootEx("Components");
  if (!versionEx.Valid() || !componentsEx.Valid()) {
    std::stringstream error;
    error << "The root Value must contain a :Version: and a :Components: Pair.";
    LogAbort(error.str().c_str());
  }
  nVersion = versionEx.As<int>();

  if (componentsEx.Size() != TypeDataCount()) {
    std::stringstream error;
    error << "Component assessment: Live and saved difference: "
          << TypeDataCount() << " live components and " << componentsEx.Size()
          << " saved components.";
    LogError(error.str().c_str());
  }

  // Display errors that show missing information and component differences.
  for (TypeId id = 0; id < componentsEx.Size(); ++id) {
    Vlk::Explorer compEx = componentsEx[id];
    AssessName(id, compEx);
    AssessSize(id, compEx);
    AssessDependencies(id, compEx);
  }
}

void SaveComponentsFile() {
  Vlk::Value rootVal;
  rootVal("Version") = nVersion;
  Vlk::Value& componentsVal = rootVal("Components")[{TypeDataCount()}];
  for (TypeId id = 0; id < TypeDataCount(); ++id) {
    const TypeData& typeData = GetTypeData(id);
    Vlk::Value& compVal = componentsVal[id];
    compVal("Id") = id;
    compVal("Name") = typeData.mName;
    compVal("Size") = typeData.mSize;
    // Save the dependencies. We don't save dependants because they can be
    // inferred from the dependencies.
    Vlk::Value& dependenciesVal =
      compVal("Dependencies")[{typeData.mDependencies.Size()}];
    for (size_t i = 0; i < typeData.mDependencies.Size(); ++i) {
      TypeId typeId = typeData.mDependencies[i];
      const TypeData& dependencyTypeData = GetTypeData(typeId);
      dependenciesVal[i] = dependencyTypeData.mName;
    }
  }
  std::string componentsFile = Rsl::PrependResDirectory(nComponentsFilename);
  Result result = rootVal.Write(componentsFile.c_str());
  LogErrorIf(!result.Success(), result.mError.c_str());
}

} // namespace Comp
