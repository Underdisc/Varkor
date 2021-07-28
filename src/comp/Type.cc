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

} // namespace Comp
