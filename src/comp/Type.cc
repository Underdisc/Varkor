#include "Type.h"

namespace Comp {

int CreateId()
{
  static int id = 0;
  return id++;
}

Ds::Vector<TypeData> nTypeData;

const TypeData& GetTypeData(TypeId id)
{
  return nTypeData[id];
}

int TypeDataCount()
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

} // namespace Comp
